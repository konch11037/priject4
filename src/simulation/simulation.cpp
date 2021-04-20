//
// Created by konch on 4/12/21.
//
#include <fstream>
#include <iostream>

#include "../algorithms/fcfs/fcfs_algorithm.hpp"
#include "../algorithms/rr/rr_algorithm.hpp"

#include "../simulation/simulation.hpp"
#include "../types/enums.hpp"

#include "../utilities/flags/flags.hpp"

Simulation::Simulation(FlagOptions flags)
{
    // Hello!
    if (flags.scheduler == "FCFS")
    {
        // Create a FCFS scheduling algorithm
        this->scheduler = std::make_shared<FCFSScheduler>();
    }
    else if (flags.scheduler == "RR")
    {
        // Create a RR scheduling algorithm
        this->scheduler = std::make_shared<RRScheduler>(flags.time_slice);
    }
    this->flags = flags;
    this->logger = Logger(flags.verbose, flags.per_thread, flags.metrics);
}

void Simulation::run()
{
    this->read_file(this->flags.filename);

    while (!this->events.empty())
    {
        auto event = this->events.top();
        this->events.pop();

        // Invoke the appropriate method in the simulation for the given event type.

        switch (event->type)
        {
            case PROCESS_ARRIVED:
                this->handle_process_arrived(event);
                break;

            case PROCESS_DISPATCH_COMPLETED:
                this->handle_dispatch_completed(event);
                break;

            case CPU_BURST_COMPLETED:
                this->handle_cpu_burst_completed(event);
                break;

            case IO_BURST_COMPLETED:
                this->handle_io_burst_completed(event);
                break;
            case PROCESS_COMPLETED:
                this->handle_process_completed(event);
                break;

            case PROCESS_PREEMPTED:
                this->handle_process_preempted(event);
                break;

            case DISPATCHER_INVOKED:
                this->handle_dispatcher_invoked(event);
                break;
        }

        // If this event triggered a state change, print it out.
        if (event->thread && event->thread->current_state != event->thread->previous_state)
        {
            this->logger.print_state_transition(event, event->thread->previous_state, event->thread->current_state);
        }
        this->system_stats.total_time = event->time;
        event.reset();
    }
    // We are done!

    std::cout << "SIMULATION COMPLETED!\n\n";

    for (auto entry : this->processes)
    {
        this->logger.print_per_thread_metrics(entry.second);
    }

    logger.print_simulation_metrics(this->calculate_statistics());
}

//==============================================================================
// Event-handling methods
//==============================================================================

void Simulation::handle_process_arrived(const std::shared_ptr<Event> event)
{
    event->thread->set_state(READY, event->time);
    if (this->active_thread == NULL) {
        events.push(event->eventCopy(DISPATCHER_INVOKED));
    }
    else this->scheduler->add_to_ready_queue(event->thread);


}

void Simulation::handle_dispatch_completed(const std::shared_ptr<Event> event)
{
    auto newEvent = event->eventCopy(event->type);
    // If timeslice is less than burst time, complete a burst
    if (scheduler->time_slice >= event->thread->get_next_burst(CPU)->length || flags.scheduler =="FCFS") {

        //If the process is on the last burst, complete the process
        if (event->thread->bursts.size() <= 1) {
            event->thread->set_state(RUNNING, event->time);
            newEvent = event->eventCopy(PROCESS_COMPLETED);
            newEvent->time += newEvent->thread->get_next_burst(CPU)->length;
            system_stats.service_time += newEvent->thread->get_next_burst(CPU)->length;
        }

        // Send the process to complete a CPU/IO burst pair
        else {
            event->thread->set_state(RUNNING, event->time);
            newEvent = event->eventCopy(CPU_BURST_COMPLETED);
            newEvent->time += newEvent->thread->get_next_burst(CPU)->length;
            system_stats.service_time += newEvent->thread->get_next_burst(CPU)->length;
        }
    }
    else {
        event->thread->set_state(RUNNING,event->time);
        newEvent = event->eventCopy(PROCESS_PREEMPTED);
        newEvent->time += scheduler->time_slice;
        system_stats.service_time += scheduler->time_slice;
    }

    events.push(newEvent);

}

void Simulation::handle_cpu_burst_completed(const std::shared_ptr<Event> event)
{
    // Calc the CPU burst time and 'advace the clock' to that point
    event->thread->set_state(BLOCKED, event->time);

    event->thread->pop_next_burst(CPU);

    //Send the current process to BLOCKED and free the CPU
    auto newEvent = event->eventCopy(IO_BURST_COMPLETED);
    newEvent->time += newEvent->thread->get_next_burst(IO)->length;
    system_stats.io_time += newEvent->thread->get_next_burst(IO)->length;

    events.push(newEvent);

    //See if there is events on the ready cue, and create and event with those threads
    if (!scheduler->empty()) {
        std::shared_ptr<SchedulingDecision> fromReadyThread = scheduler->get_next_thread();
        // Making shared pointers is not worth the effort
        this->events.push(std::make_shared<Event>(DISPATCHER_INVOKED, event->time, this->event_num, fromReadyThread->thread, fromReadyThread));
        this->event_num++;
    }

}

void Simulation::handle_io_burst_completed(const std::shared_ptr<Event> event)
{
    this->active_thread = NULL;
    event->thread->pop_next_burst(IO);
    event->thread->set_state(READY, event->time);
    auto newEvent = event->eventCopy(DISPATCHER_INVOKED);
    this->scheduler->add_to_ready_queue(newEvent->thread);

    //See if there is events on the ready cue, and create and event with those threads
    if (scheduler->size() == 1 && events.empty()) {
        std::shared_ptr<SchedulingDecision> fromReadyThread = scheduler->get_next_thread();
        // Making shared pointers is not worth the effort
        this->events.push(std::make_shared<Event>(DISPATCHER_INVOKED, event->time, this->event_num, fromReadyThread->thread, fromReadyThread));
        this->event_num++;
    }
}

void Simulation::handle_process_completed(const std::shared_ptr<Event> event)
{
    this->active_thread = NULL;
    //See if there is events on the ready cue, and create and event with those threads
    if (!scheduler->empty()) {
        std::shared_ptr<SchedulingDecision> fromReadyThread = scheduler->get_next_thread();
        // Making shared pointers is not worth the effort
        this->events.push(std::make_shared<Event>(DISPATCHER_INVOKED, event->time, this->event_num, fromReadyThread->thread, fromReadyThread));
        this->event_num++;
    }
    event->thread->pop_next_burst(CPU);
    event->thread->set_state(EXIT, event->time);


     // Figure where to put system stats
    std::shared_ptr<Thread> thready = event->thread;
    int id = 0;
    switch (thready->priority) {
        case SYSTEM:
            id = 0;
            break;
        case INTERACTIVE:
            id = 1;
            break;
        case NORMAL:
            id = 2;
            break;
        case BATCH:
            id = 3;
            break;
    }
    system_stats.avg_thread_turnaround_times[id] += thready->turnaround_time();
    system_stats.avg_thread_response_times[id] += thready->response_time();
    system_stats.thread_counts[id]++;
}

void Simulation::handle_process_preempted(const std::shared_ptr<Event> event)
{
    // Goals:
    // decrease the thread burst by the time slice
    // add the event thread to the ready cqueue
    this->active_thread = NULL;
    event->thread->set_state(READY, event->time);
    auto newEvent = event->eventCopy(DISPATCHER_INVOKED);
    newEvent->thread->bursts.front()->update_time(scheduler->time_slice);
    newEvent->thread->service_time += scheduler->time_slice;
    scheduler->add_to_ready_queue(newEvent->thread);

    //See if there is events on the ready cue, and create and event with those threads
    if (!scheduler->empty()) {
        std::shared_ptr<SchedulingDecision> fromReadyThread = scheduler->get_next_thread();
        // Making shared pointers is not worth the effort
        this->events.push(std::make_shared<Event>(DISPATCHER_INVOKED, event->time, this->event_num, fromReadyThread->thread, fromReadyThread));
        this->event_num++;
    }

}

void Simulation::handle_dispatcher_invoked(const std::shared_ptr<Event> event)
{
    auto newEvent = event->eventCopy(PROCESS_DISPATCH_COMPLETED);
    // Adding overhead
    newEvent->time += process_switch_overhead;
    system_stats.dispatch_time += process_switch_overhead;

    active_thread = event->thread;
    //create scheduler event explanation
    event->scheduling_decision->explanation = "Selected from " + std::to_string(scheduler->size()+1)
            + " processes. Will run to completion of burst.";
    events.push(newEvent);
}

//==============================================================================
// Utility methods
//==============================================================================

SystemStats Simulation::calculate_statistics()
{
    //todo
    // Calculate times
    system_stats.total_idle_time = system_stats.total_time - system_stats.service_time - system_stats.dispatch_time;
    system_stats.total_cpu_time = system_stats.total_time - system_stats.total_idle_time;

    // Calculate percentages
    system_stats.cpu_utilization = 100 * float(system_stats.total_cpu_time) / system_stats.total_time;
    system_stats.cpu_efficiency = 100 * float(system_stats.total_cpu_time - system_stats.dispatch_time) / system_stats.total_time;

    // Calculate averages for the process types
    for (int i = 0; i < 4; ++i) {
        if (system_stats.thread_counts[i] != 0) {
            system_stats.avg_thread_response_times[i] =
                    system_stats.avg_thread_response_times[i] / system_stats.thread_counts[i];
            system_stats.avg_thread_turnaround_times[i] =
                    system_stats.avg_thread_turnaround_times[i] / system_stats.thread_counts[i];
        }
    }
    return this->system_stats;
}

void Simulation::add_event(std::shared_ptr<Event> event)
{
    if (event != nullptr)
    {
        this->events.push(event);
    }
}

void Simulation::read_file(const std::string filename)
{
    std::ifstream input_file(filename.c_str());

    if (!input_file)
    {
        std::cerr << "Unable to open simulation file: " << filename << std::endl;
        throw(std::logic_error("Bad file."));
    }

    int num_processes;
    int thread_switch_overhead; // This is discarded for this semester

    input_file >> num_processes >> thread_switch_overhead >> this->process_switch_overhead;

    for (int proc = 0; proc < num_processes; ++proc)
    {
        auto process = read_process(input_file);

        this->processes[process->process_id] = process;
    }
}

std::shared_ptr<Process> Simulation::read_process(std::istream &input)
{
    int process_id, priority;
    int num_threads;

    input >> process_id >> priority >> num_threads;

    auto process = std::make_shared<Process>(process_id, (ProcessPriority)priority);

    // iterate over the threads
    for (int thread_id = 0; thread_id < num_threads; ++thread_id)
    {
        process->threads.emplace_back(read_thread(input, thread_id, process_id, (ProcessPriority)priority));
    }

    return process;
}

std::shared_ptr<Thread> Simulation::read_thread(std::istream &input, int thread_id, int process_id, ProcessPriority priority)
{
    // Stuff
    int arrival_time;
    int num_cpu_bursts;

    input >> arrival_time >> num_cpu_bursts;

    auto thread = std::make_shared<Thread>(arrival_time, thread_id, process_id, priority);

    for (int n = 0, burst_length; n < num_cpu_bursts * 2 - 1; ++n)
    {
        input >> burst_length;

        BurstType burst_type = (n % 2 == 0) ? BurstType::CPU : BurstType::IO;

        thread->bursts.push(std::make_shared<Burst>(burst_type, burst_length));
    }

    this->events.push(std::make_shared<Event>(EventType::PROCESS_ARRIVED, thread->arrival_time, this->event_num, thread, nullptr));
    this->event_num++;

    return thread;
}

#include "simulation.hpp"
