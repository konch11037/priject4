//
// Created by konch on 4/12/21.
//
#include "rr_algorithm.hpp"

#include <cassert>
#include <stdexcept>
#include <sstream>

/*
    Here is where you should define the logic for the round robin algorithm.
*/

RRScheduler::RRScheduler(int slice) {
    if (slice > 0) time_slice = slice;
    else time_slice = 3;
}

std::shared_ptr<SchedulingDecision> RRScheduler::get_next_thread() {
    auto temp = std::make_shared<SchedulingDecision>();
    temp->thread = rrQueue.front();
    rrQueue.pop();
    return temp;

}

void RRScheduler::add_to_ready_queue(std::shared_ptr<Thread> thread) {
    rrQueue.push(thread);
}

size_t RRScheduler::size() const {
    return rrQueue.size();
}

#include "rr_algorithm.hpp"
