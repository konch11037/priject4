//
// Created by konch on 4/12/21.
//
#include "fcfs_algorithm.hpp"

#include <cassert>
#include <stdexcept>

#define FMT_HEADER_ONLY
#define FMT_HEADER_ONLY
//#include "utilities/fmt/format.h"

/*
    Here is where you should define the logic for the FCFS algorithm.
*/

FCFSScheduler::FCFSScheduler(int slice) {
    if (slice != -1) {
        throw("FCFS must have a timeslice of -1");
    }
}

std::shared_ptr<SchedulingDecision> FCFSScheduler::get_next_thread() {
    auto temp = std::make_shared<SchedulingDecision>();
    temp->thread = fcfsQueue.front();
    fcfsQueue.pop();
    return temp;
}

void FCFSScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {
    fcfsQueue.push(thread);
}

size_t FCFSScheduler::size() const {
    return fcfsQueue.size();
}
#include "fcfs_algorithm.hpp"
