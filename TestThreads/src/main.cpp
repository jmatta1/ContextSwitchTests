#include<time.h>
#include<iostream>
#include<string>
#include<boost/bind.hpp>
#include<thread>
#include<condition_variable>
#include<mutex>
#include<atomic>
#include<chrono>

static const int iterCount = 1000000;
static const clockid_t clockType = CLOCK_REALTIME;

std::mutex waitMutex;
std::atomic_bool tr1(false), tr2(false), tr3(false), tr4(false);
std::atomic<unsigned long long> diffSum(0ULL);
std::atomic<unsigned long long> callTime(0ULL);
std::atomic<unsigned long long> diffCount(0ULL);

void ping(std::condition_variable& waitCond, std::condition_variable& startCond, struct timespec& start, struct timespec& stop)
{   
    for(int i=0; i<iterCount; ++i)
    {
        std::unique_lock<std::mutex> lock(waitMutex);
        //std::cout<<"Blah ping "<<diffCount<<std::endl;
        while(!tr1.load())
        {
            waitCond.wait(lock);
            clock_gettime(clockType, &stop);
        }
        diffSum += (1000000000ULL*(stop.tv_sec - start.tv_sec) + (stop.tv_nsec-start.tv_nsec));
        tr1.store(false);
        clock_gettime(clockType, &start);
        clock_gettime(clockType, &stop);
        callTime += (1000000000ULL*(stop.tv_sec - start.tv_sec) + (stop.tv_nsec-start.tv_nsec));
        diffCount.fetch_add(1);
        tr2.store(true);
        startCond.notify_one();
        clock_gettime(clockType, &start);
    }
}

void pong(std::condition_variable& waitCond, std::condition_variable& startCond, struct timespec& start, struct timespec& stop)
{
    for(int i=0; i<iterCount; ++i)
    {
        std::unique_lock<std::mutex> lock(waitMutex);
        //std::cout<<"Blah pong "<<diffCount<<std::endl;
        while(!tr2.load())
        {
            waitCond.wait(lock);
            clock_gettime(clockType, &stop);
        }
        diffSum += (1000000000ULL*(stop.tv_sec - start.tv_sec) + (stop.tv_nsec-start.tv_nsec));
        tr2.store(false);
        clock_gettime(clockType, &start);
        clock_gettime(clockType, &stop);
        callTime += (1000000000ULL*(stop.tv_sec - start.tv_sec) + (stop.tv_nsec-start.tv_nsec));
        diffCount.fetch_add(1);
        tr3.store(true);
        startCond.notify_one();
        clock_gettime(clockType, &start);
    }
}

void clong(std::condition_variable& waitCond, std::condition_variable& startCond, struct timespec& start, struct timespec& stop)
{
    for(int i=0; i<iterCount; ++i)
    {
        std::unique_lock<std::mutex> lock(waitMutex);
        //std::cout<<"Blah clong "<<diffCount<<std::endl;
        while(!tr3.load())
        {
            waitCond.wait(lock);
            clock_gettime(clockType, &stop);
        }
        diffSum += (1000000000ULL*(stop.tv_sec - start.tv_sec) + (stop.tv_nsec-start.tv_nsec));
        tr3.store(false);
        clock_gettime(clockType, &start);
        clock_gettime(clockType, &stop);
        callTime += (1000000000ULL*(stop.tv_sec - start.tv_sec) + (stop.tv_nsec-start.tv_nsec));
        diffCount.fetch_add(1);
        tr4.store(true);
        startCond.notify_one();
        clock_gettime(clockType, &start);
    }
}

int main()
{
    std::condition_variable cond1, cond2, cond3, cond4;
    struct timespec start, stop;

    std::thread t1(boost::bind(&ping, boost::ref(cond1), boost::ref(cond2), boost::ref(start), boost::ref(stop)));
    std::thread t2(boost::bind(&pong, boost::ref(cond2), boost::ref(cond3), boost::ref(start), boost::ref(stop)));
    std::thread t3(boost::bind(&clong, boost::ref(cond3), boost::ref(cond4), boost::ref(start), boost::ref(stop)));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    for(int i=0; i<iterCount; ++i)
    {
        std::unique_lock<std::mutex> lock(waitMutex);
        tr1.store(true);
        cond1.notify_one();
        clock_gettime(clockType, &start);

        //std::cout<<"main "<<diffCount<<std::endl;
        while(!tr4.load())
        {
            cond4.wait(lock);
            clock_gettime(clockType, &stop);
        }
        diffSum += (1000000000ULL*(stop.tv_sec - start.tv_sec) + (stop.tv_nsec-start.tv_nsec));
        tr4.store(false);
        clock_gettime(clockType, &start);
        clock_gettime(clockType, &stop);
        callTime += (1000000000ULL*(stop.tv_sec - start.tv_sec) + (stop.tv_nsec-start.tv_nsec));
        diffCount.fetch_add(1);
    }

    t1.join();
    t2.join();
    t3.join();

    std::cout << "Thread Context Switch Test Complete." << std::endl;

    std::cout << "    Total Thread Context Switch Time Difference Was: " << diffSum << "ns across " << diffCount <<
                 " Switches.\n\tAverage Time Difference Was: "
              << (static_cast<double>(diffSum) / static_cast<double>(diffCount))<<" nanoseconds"<<std::endl;

    std::cout << "    Total Call Time Difference Was: " << callTime << "ns across " << diffCount <<
                 " Call Pairs.\n\tAverage Call Time Difference Was: "
              << (static_cast<double>(callTime) / static_cast<double>(diffCount))<<" nanoseconds"<<std::endl;

    std::cout << "\n    The average, clock_gettime call compensated, thread context switch time is: " <<
                  (static_cast<double>(diffSum-callTime) / static_cast<double>(diffCount))<<" nanoseconds"<<std::endl;

    return EXIT_SUCCESS;
}

