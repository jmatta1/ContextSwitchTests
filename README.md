# ContextSwitchTests

A pair of programs to test the speed of context switches between Boost::Fibers (userland cooperative multi-tasking 'threads') and std::Threads. Fibers all run in the same thread and cooperatively yield time to the next fiber either with a fiber blocking call or a yield. Contrariwise, std::threads are full blown OS provided threads that run on seperate cores and stop when they hit a blocking operation or the OS preempts them.

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
This yields 4,000,000 context switches to time, as well as 4,000,000 points of data for extracting the time taken by `clock_gettime(...)`.
The calculation of that value may be a bit off, but it is probably pretty close and represents a noticable percentage of the time of the fiber context switch.
I suspect the `clock_gettime` call time differences between threaded and fibered versions comes from the fact that because the calls are in different threads and thus probably different cores, causing cache misses.

## Example Results For Fibers
On my Mid-2015 Macbook Pro, running `time ./testFiber 50 1000000` yielded:
```
Fiber Context Switch Test Complete.  Ran With 50 Fibers and 1000000 Iterations
    Total Fiber Context Switch Time Difference Was: 2849903000ns across 50000000 Switches.
  Average Time Difference Was: 56.9981 nanoseconds
    Total Call Time Difference Was: 1318719000ns across 50000000 Call Pairs.
  Average Call Time Difference Was: 26.3744 nanoseconds

The average, clock_gettime call compensated, fiber context switch time is: 30.6237 nanoseconds

real  0m9.559s
user  0m9.537s
sys 0m0.014s
```

Running `time ./testFiber 4 1000000` (defaults) yielded:
```
Fiber Context Switch Test Complete.  Ran With 4 Fibers and 1000000 Iterations
    Total Fiber Context Switch Time Difference Was: 303192000ns across 4000000 Switches.
  Average Time Difference Was: 75.798 nanoseconds
    Total Call Time Difference Was: 104544000ns across 4000000 Call Pairs.
  Average Call Time Difference Was: 26.136 nanoseconds

The average, clock_gettime call compensated, fiber context switch time is: 49.662 nanoseconds

real  0m0.840s
user  0m0.830s
sys 0m0.007s
```

## Example Results For Threads

On my Mid-2015 Macbook Pro, running `time ./testThread 50 1000000` yielded:
```
TBA
```

Running `time ./testThread 4 1000000` (defaults) yielded:
```
TBA
```
