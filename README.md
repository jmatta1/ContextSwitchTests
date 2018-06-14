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

Both programs take two optional command line arguments. The arguments are `numFibers`/`numThreads` and `numIterations`. To specify `numIterations`, `numFibers`/`numThreads` must be specified first. If `numIterations` is not specified it defaults to 1,000,000. If `numFibers`/`numThreads` is not specified, it defaults to 4.

By default both test codes run 1,000,000 switches between 4 different multi-tasking types (Fibers or Threads).
This yields 4,000,000 context switches to time, as well as 4,000,000 points of data for extracting the time taken by the two calls to `__rdtsc`.
The calculation of that value may be a bit off, but it is probably pretty close and represents a noticable percentage of the time of the fiber context switch.
I suspect the `__rdtsc` call time differences between threaded and fibered versions comes from the fact that because the data set by calls are in different threads and thus probably different cores, causing cache misses.

## Example Results For Fibers
On my Mid-2015 Macbook Pro, running `time ./testFiber 50 1000000` yielded:
```
Fiber Context Switch Test Complete.  Ran With 50 Fibers and 1000000 Iterations
    Total Fiber Context Switch Time Difference Was: 3364196411 cycles across 50000000 Switches.
  Average Time Difference Was: 67.2839 cycles
    Total Call __rdtsc Time Difference Was: 863533911 cycles across 50000000 Call Pairs.
  Average Call __rdtsc Time Difference Was: 17.2707 cycles

The average, __rdtsc call compensated, fiber context switch time is: 50.0133 cycles

real  0m2.442s
user  0m2.432s
sys   0m0.008s
```

Running `time ./testFiber` (defaults to 4 fibers and 1,000,000 iterations) yielded:
```
Fiber Context Switch Test Complete.  Ran With 4 Fibers and 1000000 Iterations
    Total Fiber Context Switch Time Difference Was: 488635036 cycles across 4000000 Switches.
  Average Time Difference Was: 122.159 cycles
    Total Call __rdtsc Time Difference Was: 68659741 cycles across 4000000 Call Pairs.
  Average Call __rdtsc Time Difference Was: 17.1649 cycles

The average, __rdtsc call compensated, fiber context switch time is: 104.994 cycles

real  0m0.270s
user  0m0.263s
sys   0m0.006s
```

## Example Results For Threads

On my Mid-2015 Macbook Pro, running `time ./testThread 50 1000000` yielded:
```
Thread Context Switch Test Complete.  Ran With 50 Threads and 1000000 Iterations
    Total Thread Context Switch Time Difference Was: 240188965971 cycles across 50000000 Switches.
  Average Time Difference Was: 4803.78 cycles
    Total Call __rdtsc Time Difference Was: 1090830363 cycles across 50000000 Call Pairs.
  Average Call __rdtsc Time Was: 21.8166 cycles

    The average, __rdtsc call, thread context switch time is: 4781.96 cycles

real  2m57.662s
user  1m1.743s
sys   2m42.043s
```

Running `time ./testThread` (defaults to 4 threads and 1,000,000 iterations) yielded:
```
Thread Context Switch Test Complete.  Ran With 4 Threads and 1000000 Iterations
    Total Thread Context Switch Time Difference Was: 19163906885 cycles across 4000000 Switches.
  Average Time Difference Was: 4790.98 cycles
    Total Call __rdtsc Time Difference Was: 110837473 cycles across 4000000 Call Pairs.
  Average Call __rdtsc Time Was: 27.7094 cycles

    The average, __rdtsc call compensated, thread context switch time is: 4763.27 cycles

real  0m14.007s
user  0m4.764s
sys   0m11.334s
```
