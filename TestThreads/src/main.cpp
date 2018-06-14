#include<time.h>
#include<chrono>
#include<vector>
#include<iostream>
#include<sstream>
#include<thread>
#include<mutex>
#include<atomic>
#include<condition_variable>
#include<boost/bind.hpp>

static const clockid_t clockType = CLOCK_REALTIME;

unsigned long long iterCount = 1000000;
std::mutex waitMutex;
std::atomic_bool* threadRun;
std::condition_variable* condVars;
std::atomic<unsigned long long> diffSum(0ULL);
std::atomic<unsigned long long> callTime(0ULL);
std::atomic<unsigned long long> loadTime(0ULL);
std::atomic<unsigned long long> diffCount(0ULL);

int checkCmdLineArgs(int argc, char* argv[], int& threadCount, unsigned long long& iterationCount);
void threadFunction(int index, struct timespec& start, struct timespec& stop);
unsigned long long timeDiff(struct timespec start, struct timespec stop);


int main(int argc, char* argv[])
{
    int threadCount;
    if(checkCmdLineArgs(argc, argv, threadCount, iterCount) != 0)
    {
        return 1;
    }

    threadRun = new std::atomic_bool[threadCount];
    for(int i=0; i<threadCount; ++i)
    {
        threadRun[i].store(false);
    }
    condVars = new std::condition_variable[threadCount];

    struct timespec start, stop;

    std::vector<std::thread> threadList;

    int lastIndex = (threadCount - 1);

    for(int i=0; i<lastIndex; ++i)
    {
        threadList.emplace_back(std::thread(boost::bind(&threadFunction, i, boost::ref(start), boost::ref(stop))));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    for(int i=0; i<iterCount; ++i)
    {
        std::unique_lock<std::mutex> lock(waitMutex);
        threadRun[0].store(true);
        condVars[0].notify_one();
        clock_gettime(clockType, &start);

        //std::cout<<"main "<<diffCount<<std::endl;
        while(!threadRun[lastIndex].load())
        {
            condVars[lastIndex].wait(lock);
            clock_gettime(clockType, &stop);
        }
        diffSum += timeDiff(start, stop);
        threadRun[lastIndex].store(false);
        clock_gettime(clockType, &start);
        clock_gettime(clockType, &stop);
        callTime += timeDiff(start, stop);
        clock_gettime(clockType, &start);
        while(threadRun[lastIndex].load())
        {//should never execute
            std::this_thread::yield();
        }
        clock_gettime(clockType, &stop);
        loadTime += timeDiff(start, stop);
        diffCount.fetch_add(1);
    }

    for(int i=0; i<lastIndex; ++i)
    {
        threadList[i].join();
    }

    delete[] threadRun;
    delete[] condVars;

    std::cout << "\n\nThread Context Switch Test Complete." << "  Ran With "
              << threadCount << " Threads and " << iterCount << " Iterations"
              << std::endl;

    double avgTimeDiff = (static_cast<double>(diffSum) / static_cast<double>(diffCount));
    double avgCallTime = (static_cast<double>(callTime) / static_cast<double>(diffCount));
    double avgLoadTime = ((static_cast<double>(loadTime) / static_cast<double>(diffCount)) - avgCallTime);

    std::cout << "    Total Thread Context Switch Time Difference Was: " << diffSum << "ns across " << diffCount <<
                 " Switches.\n\tAverage Time Difference Was: "
              << avgTimeDiff <<" nanoseconds"<<std::endl;

    std::cout << "    Total Call clock_gettime Time Difference Was: " << callTime << "ns across " << diffCount <<
                 " Call Pairs.\n\tAverage Call clock_gettime Time Was: "
              << avgCallTime <<" nanoseconds"<<std::endl;

    std::cout << "    Total while(atomic_bool.load()) Time Difference Was: " << loadTime << "ns across " << diffCount <<
                 " Call Pairs.\n\tAverage, clock_gettime call compensated, while(atomic_bool.load()) Time Was: "
              << avgLoadTime <<" nanoseconds"<<std::endl;

    std::cout << "\n    The average, clock_gettime call and while(atomic_bool.load()) compensated, thread context switch time is: " <<
                  (avgTimeDiff - avgCallTime - avgLoadTime)<<" nanoseconds\n\n"<<std::endl;

    return 0;
}

void threadFunction(int index, struct timespec& start, struct timespec& stop)
{   
    for(int i=0; i<iterCount; ++i)
    {
        std::unique_lock<std::mutex> lock(waitMutex);
        //std::cout<<"Blah ping "<<diffCount<<std::endl;
        while(!threadRun[index].load())
        {
            condVars[index].wait(lock);
            clock_gettime(clockType, &stop);
        }
        diffSum += timeDiff(start, stop);
        threadRun[index].store(false);
        clock_gettime(clockType, &start);
        clock_gettime(clockType, &stop);
        callTime += timeDiff(start, stop);
        clock_gettime(clockType, &start);
        while(threadRun[index].load())
        {//should never execute
            std::this_thread::yield();
        }
        clock_gettime(clockType, &stop);
        loadTime += timeDiff(start, stop);
        diffCount.fetch_add(1);
        threadRun[index+1].store(true);
        condVars[index+1].notify_one();
        clock_gettime(clockType, &start);
    }
}

unsigned long long timeDiff(struct timespec start, struct timespec stop)
{
    return ((1000000000ULL*(stop.tv_sec - start.tv_sec)) + (stop.tv_nsec-start.tv_nsec));
}

void printHelp(const std::string& call)
{
    std::cout<<"Usage:\n    " << call << " [ <Num Threads To Use> [Num Iterations To Run] ]\n"<<std::endl;
    std::cout<<"  <Num Threads To Use> must be in the range [2, 1000] (inclusive) - defaults to 50"<<std::endl;
    std::cout<<"  [Num Iterations To Run] must be in the range [3, 500000000] (inclusive) - defaults to 1000000"<<std::endl;
}

int checkCmdLineArgs(int argc, char* argv[], int& threadCount, unsigned long long& iterationCount)
{
    if(argc == 1) // left both to defaults
    {
        iterationCount = 1000000;
        threadCount = 4;
    }
    else if(argc == 2) // specified a thread count but not an iteration count
    {
        iterationCount = 1000000;
        std::istringstream conv;
        conv.str(argv[1]);
        conv >> threadCount;
        if((threadCount <2) || (threadCount > 1000))
        {
            printHelp(argv[0]);
            return 1;
        }
    }
    else if(argc == 3) // specified both a thread and iteration count
    {
        std::istringstream conv;
        conv.str(argv[1]);
        conv >> threadCount;
        if((threadCount < 2) || (threadCount > 1000))
        {
            printHelp(argv[0]);
            return 1;
        }
        conv.clear();
        conv.str(argv[2]);
        conv >> iterationCount;
        if((iterationCount < 3) || (iterationCount > 500000000))
        {
            printHelp(argv[0]);
            return 1;
        }
    }
    else // specified more cmd line options that we accept
    {
        printHelp(argv[0]);
        return 1;
    }
    return 0;
}