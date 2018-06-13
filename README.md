# ContextSwitchTests

A pair of programs to test the speed of context switches between Boost::Fibers (userland cooperative multi-tasking 'threads' that all run in the same thread and cooperatively yield time to the next fiber (as opposed to having the OS preempt them).

## Prerequisites
 - Linux or Mac System (probably):
   - I use calls to `clock_gettime(CLOCK_REALTIME,...` I am uncertain if this is supported on windows
 - Boost: Provides the fiber library.
   - On mac this can be obtained with `brew install boost` on linux something like `sudo dnf install boost`, `sudo yum install boost`, or `sudo apt-get install boost` should do the job.
 - pthreads: Necessary for doing anything with threads.
   - If this is not already installed on your linux or MaxOS system then something is very wrong.
 - C++11 compiler: Necessary for std::threads and boost::fiber

## Build
Simply run `make` in the base directory. This will call cmake in the subdirectories, build the executables with `-O3`, and move the resultant executables to the base directory.

## Execution
Simply run `./testFiber` to test the time of context switches between `Boost::Fiber`.
Simply run `./testThread` to test the time of context switches between `std::thread`.
Both programs run 1,000,000 switches between 4 different multi-tasking types (Fibers or Threads).
This yields 4,000,000 context switches to time, as well as 4,000,000 points of data for extracting the time taken by `clock_gettime(...)`. The calculation of that value may be a bit off, but it is probably pretty close and represents a noticable percentage of the time of the context switch.
## Example Results
On my Mid-2015 Macbook Pro, running `time ./testFiber` yields:
```
Fiber Context Switch Test Complete.
    Total Fiber Context Switch Time Difference Was: 386241000ns across 4000000 Switches.
  Average Time Difference Was: 96.5602 nanoseconds
    Total Call Time Difference Was: 106640000ns across 4000000 Call Pairs.
  Average Call Time Difference Was: 26.66 nanoseconds

    The average, clock_gettime call compensated, fiber context switch time is: 69.9002 nanoseconds

real  0m1.278s
user  0m1.269s
sys 0m0.007s
```
On the same system, running `time ./testThread` yielded:
```
Thread Context Switch Test Complete.
    Total Thread Context Switch Time Difference Was: 6979065000ns across 4000000 Switches.
  Average Time Difference Was: 1744.77 nanoseconds
    Total Call Time Difference Was: 140654000ns across 4000000 Call Pairs.
  Average Call Time Difference Was: 35.1635 nanoseconds

    The average, clock_gettime call compensated, thread context switch time is: 1709.6 nanoseconds

real  0m14.613s
user  0m5.538s
sys 0m11.281s
```