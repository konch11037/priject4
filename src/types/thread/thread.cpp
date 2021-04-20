//
// Created by konch on 4/12/21.
//

#include "thread.hpp"

#include <cassert>
#include <cstddef>
#include <stdexcept>
//#include "types/thread/thread.hpp"

void Thread::set_ready(int time) {
    ThreadState temp = current_state;

//    if (current_state == BLOCKED) {
//        service_time += time - state_change_time;
//    }
    current_state = READY;
    previous_state = temp;
    state_change_time = time;

}

void Thread::set_running(int time) {
    ThreadState temp = current_state;
    if (startTimeFlag) {
        start_time = time;
        startTimeFlag = false;
    }
    current_state = RUNNING;
    previous_state = temp;
    state_change_time = time;
}

void Thread::set_blocked(int time) {
    ThreadState temp = current_state;
    //io_time += time - state_change_time;
    current_state = BLOCKED;
    previous_state = temp;
    state_change_time = time;
}

void Thread::set_finished(int time) {
    ThreadState temp = current_state;
    current_state = EXIT;
    previous_state = temp;
    state_change_time = time;
    end_time = time;
}

int Thread::response_time() const {
    return start_time - arrival_time;
}

int Thread::turnaround_time() const {
    return end_time - arrival_time;
}

void Thread::set_state(ThreadState state, int time) {
    switch (state) {
        case EXIT:
            set_finished(time);
            break;
        case BLOCKED:
            set_blocked(time);
            break;
        case RUNNING:
            set_running(time);
            break;
        case READY:
            set_ready(time);
            break;
    }
}

std::shared_ptr<Burst> Thread::get_next_burst(BurstType type) {
    if (bursts.front()->burst_type == type) return bursts.front();
    return nullptr;
}

std::shared_ptr<Burst> Thread::pop_next_burst(BurstType type) {
    if (bursts.front()->burst_type == type) {
        std::shared_ptr<Burst> temp = bursts.front();
        if (type == CPU) service_time += temp->length;
        if (type == IO) io_time += temp->length;

        bursts.pop();
        return temp;
    }
    return nullptr;
}
