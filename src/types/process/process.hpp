//
// Created by konch on 4/12/21.
//

#ifndef PRIJECT4_PROCESS_HPP
#define PRIJECT4_PROCESS_HPP

#include <memory>
#include <vector>

#include "../enums.hpp"
#include "../thread/thread.hpp"

/*
    Process:
        A simple class representing a process. It contains all the
        information a process needs, but you can add more if you think
        it is necessary.
*/

class Process {
public:
    //==================================================
    //  Member variables
    //==================================================

    /*
        process_id:
            The process's ID. These are unique, and each process is assigned
            one in the simulation file.
    */
    int process_id;

    /*
        priority:
            The process's priority. Each project is assigned a priority
            from what is provided in the simulation file.
    */
    ProcessPriority priority;

    /*
        threads:
            A vector of the process's threads.
    */
    std::vector<std::shared_ptr<Thread>> threads;

    //==================================================
    //  Member functions
    //==================================================

    /*
        Process(pid, priority):
            A constructor for a new process object. We give it a process ID and priority,
            and a new process with that information is create.
    */
    Process(int pid, ProcessPriority priority) : process_id(pid), priority(priority) {}
};

#endif
