cmake_minimum_required(VERSION 3.19)
project(priject4)

set(CMAKE_CXX_STANDARD 14)

add_executable(priject4 src/main.cpp src/algorithms/scheduling_algorithm.hpp src/algorithms/fcfs/fcfs_algorithm.cpp src/algorithms/fcfs/fcfs_algorithm.hpp src/algorithms/rr/rr_algorithm.cpp src/algorithms/rr/rr_algorithm.hpp src/simulation/simulation.cpp src/simulation/simulation.hpp src/types/burst/burst.cpp src/types/burst/burst.hpp src/types/event/event.hpp src/types/process/process.hpp src/types/scheduling_decision/scheduling_decision.hpp src/types/system_stats/system_stats.hpp src/types/thread/thread.cpp src/types/thread/thread.hpp src/types/enums.hpp src/utilities/flags/flags.cpp src/utilities/flags/flags.hpp src/utilities/logger/logger.cpp src/utilities/logger/logger.hpp src/utilities/fmt/format.h src/utilities/fmt/core.h src/utilities/fmt/format-inl.h src/utilities/fmt/format.cpp)