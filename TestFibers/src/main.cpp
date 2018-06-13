#include<time.h>
#include<iostream>
#include<string>
#include<boost/bind.hpp>
#include<boost/fiber/all.hpp>

typedef boost::fibers::buffered_channel< std::string > fifo_t;

static const int iterCount = 1000000;
static const clockid_t clockType = CLOCK_REALTIME;
//static const clockid_t clockType = CLOCK_MONOTONIC;
//static const clockid_t clockType = CLOCK_PROCESS_CPUTIME_ID;
//static const clockid_t clockType = CLOCK_THREAD_CPUTIME_ID;

struct timespec start, stop;

unsigned long long diffSum=0ULL;

unsigned long long callTime=0ULL;

unsigned long long diffCount=0;

void ping( fifo_t & recv_buf, fifo_t & send_buf)
{
    //boost::fibers::fiber::id id(boost::this_fiber::get_id());

    //std::cout << "fiber " <<  id << ": Starting" << std::endl;
    
    for(int i=0; i<iterCount; ++i)
    {
        send_buf.push("");
        clock_gettime(clockType, &start);
        boost::this_fiber::yield();

        std::string value;
        clock_gettime(clockType, &stop);
        diffSum += (1000000000ULL*(stop.tv_sec - start.tv_sec) + (stop.tv_nsec-start.tv_nsec));
        recv_buf.pop( value);
        clock_gettime(clockType, &start);
        clock_gettime(clockType, &stop);
        callTime += (1000000000ULL*(stop.tv_sec - start.tv_sec) + (stop.tv_nsec-start.tv_nsec));
        ++diffCount;
        //std::cout << "fiber " <<  id << ": ping received: " << value << std::endl;
        value.clear();
    }

    send_buf.close();
}

void pong( fifo_t & recv_buf, fifo_t & send_buf)
{
    //boost::fibers::fiber::id id(boost::this_fiber::get_id());

    for(int i=0; i<iterCount; ++i)
    {
        std::string value;
        clock_gettime(clockType, &stop);
        diffSum += (1000000000ULL*(stop.tv_sec - start.tv_sec) + (stop.tv_nsec-start.tv_nsec));
        recv_buf.pop( value);
        clock_gettime(clockType, &start);
        clock_gettime(clockType, &stop);
        callTime += (1000000000ULL*(stop.tv_sec - start.tv_sec) + (stop.tv_nsec-start.tv_nsec));
        ++diffCount;
        //std::cout << "fiber " <<  id << ": pong received: " << value << std::endl;
        value.clear();

        send_buf.push("");
        clock_gettime(clockType, &start);
        boost::this_fiber::yield();
    }

    send_buf.close();
}

void clong( fifo_t & recv_buf, fifo_t & send_buf)
{
    //boost::fibers::fiber::id id(boost::this_fiber::get_id());

    for(int i=0; i<iterCount; ++i)
    {
        std::string value;
        clock_gettime(clockType, &stop);
        diffSum += (1000000000ULL*(stop.tv_sec - start.tv_sec) + (stop.tv_nsec-start.tv_nsec));
        recv_buf.pop( value);
        clock_gettime(clockType, &start);
        clock_gettime(clockType, &stop);
        callTime += (1000000000ULL*(stop.tv_sec - start.tv_sec) + (stop.tv_nsec-start.tv_nsec));
        ++diffCount;
        //std::cout << "fiber " <<  id << ": clong received: " << value << std::endl;
        value.clear();

        send_buf.push("");
        clock_gettime(clockType, &start);
        boost::this_fiber::yield();
    }

    send_buf.close();
}

int main()
{
    fifo_t buf1(2), buf2(2), buf3(2), buf4(2);

    boost::fibers::fiber f1(
            boost::bind(&ping, boost::ref(buf1), boost::ref(buf2)));
    boost::fibers::fiber f2(
            boost::bind(&pong, boost::ref(buf2), boost::ref(buf3)));
    boost::fibers::fiber f3(
            boost::bind(&clong, boost::ref(buf3), boost::ref(buf4)));

    //boost::fibers::fiber::id id(boost::this_fiber::get_id());


    for(int i=0; i<iterCount; ++i)
    {
        std::string value;
        buf4.pop( value);
        clock_gettime(clockType, &stop);
        diffSum += (1000000000ULL*(stop.tv_sec - start.tv_sec) + (stop.tv_nsec-start.tv_nsec));
        clock_gettime(clockType, &start);
        clock_gettime(clockType, &stop);
        callTime += (1000000000ULL*(stop.tv_sec - start.tv_sec) + (stop.tv_nsec-start.tv_nsec));
        ++diffCount;
        //std::cout << "fiber " <<  id << ": bong received: " << value << std::endl;
        value.clear();

        buf1.push("");
        clock_gettime(clockType, &start);
        boost::this_fiber::yield();
    }

    buf1.close();

    f1.join();
    f2.join();
    f3.join();

    std::cout << "Fiber Context Switch Test Complete." << std::endl;

    std::cout << "    Total Fiber Context Switch Time Difference Was: " << diffSum << "ns across " << diffCount <<
                 " Switches.\n\tAverage Time Difference Was: "
              << (static_cast<double>(diffSum) / static_cast<double>(diffCount))<<" nanoseconds"<<std::endl;

    std::cout << "    Total Call Time Difference Was: " << callTime << "ns across " << diffCount <<
                 " Call Pairs.\n\tAverage Call Time Difference Was: "
              << (static_cast<double>(callTime) / static_cast<double>(diffCount))<<" nanoseconds"<<std::endl;

    std::cout << "\n    The average, clock_gettime call compensated, fiber context switch time is: " <<
                  (static_cast<double>(diffSum-callTime) / static_cast<double>(diffCount))<<" nanoseconds"<<std::endl;

    return EXIT_SUCCESS;
}

