//
// Created by konch on 4/12/21.
//

#ifndef PRIJECT4_ENUMS_HPP
#define PRIJECT4_ENUMS_HPP

/*
    A set of enumerated types for various things. These are useful
    because they give us nice names for things like the algorithms
    or event types, but we can treat them as their own types.
*/

enum Algorithms {
    FCFS,
    RR,
    PRIORITY,
    MLFQ
};

enum BurstType {
    CPU,
    IO
};

enum EventType {
    PROCESS_ARRIVED,
    THREAD_DISPATCH_COMPLETED,
    PROCESS_DISPATCH_COMPLETED,
    CPU_BURST_COMPLETED,
    IO_BURST_COMPLETED,
    PROCESS_COMPLETED,
    PROCESS_PREEMPTED,
    DISPATCHER_INVOKED
};

enum ProcessPriority {
    SYSTEM,
    INTERACTIVE,
    NORMAL,
    BATCH
};

enum ThreadState {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    EXIT
};

#endif