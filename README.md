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
This yields 4,000,000 context switches to time, as well as 4,000,000 points of data for extracting the time taken by `clock_gettime(...)`.
The calculation of that value may be a bit off, but it is probably pretty close and represents a noticable percentage of the time of the fiber context switch.
I suspect the `clock_gettime` call time differences between threaded and fibered versions comes from the fact that because the calls are in different threads and thus probably different cores, causing cache misses.
In the thread case it also yields an attempt to measure the time of an atomic load; however, it seems that the branch predictor kills this.


## Example Results For Fibers
On my Mid-2015 Macbook Pro, running `time ./testFiber 50 1000000` yielded:
```
Fiber Context Switch Test Complete.  Ran With 50 Fibers and 1000000 Iterations
    Total Fiber Context Switch Time Difference Was: 2770521000ns across 50000000 Switches.
  Average Time Difference Was: 55.4104 nanoseconds
    Total Call Time Difference Was: 1294879000ns across 50000000 Call Pairs.
  Average Call Time Difference Was: 25.8976 nanoseconds

The average, clock_gettime call compensated, fiber context switch time is: 29.5128 nanoseconds

real  0m6.877s
user  0m6.864s
sys 0m0.010s
```

Running `time ./testFiber` (defaults to 4 threads and 1,000,000 iterations) yielded:
```
Fiber Context Switch Test Complete.  Ran With 4 Fibers and 1000000 Iterations
    Total Fiber Context Switch Time Difference Was: 277832000ns across 4000000 Switches.
  Average Time Difference Was: 69.458 nanoseconds
    Total Call Time Difference Was: 103193000ns across 4000000 Call Pairs.
  Average Call Time Difference Was: 25.7982 nanoseconds

The average, clock_gettime call compensated, fiber context switch time is: 43.6598 nanoseconds

real  0m0.615s
user  0m0.606s
sys 0m0.007s
```

## Example Results For Threads

On my Mid-2015 Macbook Pro, running `time ./testThread 50 1000000` yielded:
```
Thread Context Switch Test Complete.  Ran With 50 Threads and 1000000 Iterations
    Total Thread Context Switch Time Difference Was: 81841969000ns across 50000000 Switches.
  Average Time Difference Was: 1636.84 nanoseconds
    Total Call clock_gettime Time Difference Was: 1391061000ns across 50000000 Call Pairs.
  Average Call clock_gettime Time Was: 27.8212 nanoseconds
    Total while(atomic_bool.load()) Time Difference Was: 1372330000ns across 50000000 Call Pairs.
  Average, clock_gettime call compensated, while(atomic_bool.load()) Time Was: -0.37462 nanoseconds

    The average, clock_gettime call and while(atomic_bool.load()) compensated, thread context switch time is: 1609.39 nanoseconds

real  2m59.500s
user  1m13.211s
sys 2m33.086s
```

Running `time ./testThread` (defaults to 4 threads and 1,000,000 iterations) yielded:
```
Thread Context Switch Test Complete.  Ran With 4 Threads and 1000000 Iterations
    Total Thread Context Switch Time Difference Was: 6750985000ns across 4000000 Switches.
  Average Time Difference Was: 1687.75 nanoseconds
    Total Call clock_gettime Time Difference Was: 121785000ns across 4000000 Call Pairs.
  Average Call clock_gettime Time Was: 30.4462 nanoseconds
    Total while(atomic_bool.load()) Time Difference Was: 113771000ns across 4000000 Call Pairs.
  Average, clock_gettime call compensated, while(atomic_bool.load()) Time Was: -2.0035 nanoseconds

    The average, clock_gettime call and while(atomic_bool.load()) compensated, thread context switch time is: 1659.3 nanoseconds

real  0m14.726s
user  0m6.007s
sys 0m11.118s
```
