//
// Created by konch on 4/12/21.
//

#ifndef PRIJECT4_FCFS_ALGORITHM_HPP
#define PRIJECT4_FCFS_ALGORITHM_HPP


#include <memory>
#include <queue>
#include "../scheduling_algorithm.hpp"

/*
    FCFSScheduler:
        A representation of a scheduling queue that uses first-come, first-served logic.

        This is a derived class from the base scheduling algorithm class.

        You are free to add any member functions or member variables that you
        feel are helpful for implementing the algorithm.
*/

class FCFSScheduler : public Scheduler {
public:

    //==================================================
    //  Member variables
    //==================================================

    // Add any member variables you may need.

    std::queue<std::shared_ptr<Thread>> fcfsQueue;
    //==================================================
    //  Member functions
    //==================================================

    FCFSScheduler(int slice = -1);

    std::shared_ptr<SchedulingDecision> get_next_thread();

    void add_to_ready_queue(std::shared_ptr<Thread> thread);

    size_t size() const;


};


#endif //PRIJECT4_FCFS_ALGORITHM_HPP
