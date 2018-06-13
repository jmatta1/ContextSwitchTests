#include<time.h>
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<boost/bind.hpp>
#include<boost/fiber/all.hpp>

typedef boost::fibers::buffered_channel< std::string > FifoType;
typedef boost::fibers::fiber FiberType;

static const clockid_t clockType = CLOCK_REALTIME;

//timing variables
struct timespec start, stop;
unsigned long long diffSum=0ULL;
unsigned long long callTime=0ULL;
unsigned long long diffCount=0;
//iteration limit (set elsewhere)
unsigned long long iterCount;

int checkCmdLineArgs(int argc, char* argv[], int& fiberCount, unsigned long long& iterationCount);
void fiberFunction( FifoType& recvBuff, FifoType& sendBuff);
unsigned long long timeDiff(struct timespec start, struct timespec stop);

int main(int argc, char* argv[])
{
    int fiberCount;
    if(checkCmdLineArgs(argc, argv, fiberCount, iterCount) != 0)
    {
        return 1;
    }
    
    // set up the fibers
    int lastBufferIndex = (fiberCount - 1);
    // generate the set of buffers
    std::vector<FifoType*> bufferVector;
    for(int i=0; i<fiberCount; ++i)
    {
        FifoType* temp = new FifoType(2);
        bufferVector.emplace_back(temp);
    }

    // generate the set of fibers
    std::vector<FiberType> fiberVector;
    for(int i=0; i<lastBufferIndex; ++i)
    {
        fiberVector.emplace_back(FiberType(boost::bind(&fiberFunction,
                                           boost::ref(*(bufferVector[i])),
                                           boost::ref(*(bufferVector[i+1])))));
    }

    //execute the main fiber
    for(unsigned long long i=0; i<iterCount; ++i)
    {
        bufferVector[0]->push("");
        clock_gettime(clockType, &start);
        boost::this_fiber::yield();

        std::string value;
        bufferVector[lastBufferIndex]->pop( value);
        clock_gettime(clockType, &stop);
        diffSum += timeDiff(start, stop);
        clock_gettime(clockType, &start);
        clock_gettime(clockType, &stop);
        callTime += timeDiff(start, stop);
        ++diffCount;
        value.clear();
    }

    bufferVector[0]->close();

    for(int i=0; i<lastBufferIndex; ++i)
    {
        fiberVector[i].join();
    }

    std::cout << "\n\nFiber Context Switch Test Complete." << "  Ran With "
              << fiberCount << " Fibers and " << iterCount << " Iterations"
              << std::endl;

    std::cout << "    Total Fiber Context Switch Time Difference Was: " << diffSum << "ns across " << diffCount <<
                 " Switches.\n\tAverage Time Difference Was: "
              << (static_cast<double>(diffSum) / static_cast<double>(diffCount))<<" nanoseconds"<<std::endl;

    std::cout << "    Total Call Time Difference Was: " << callTime << "ns across " << diffCount <<
                 " Call Pairs.\n\tAverage Call Time Difference Was: "
              << (static_cast<double>(callTime) / static_cast<double>(diffCount))<<" nanoseconds"<<std::endl;

    std::cout << "\nThe average, clock_gettime call compensated, fiber context switch time is: " <<
                  (static_cast<double>(diffSum-callTime) / static_cast<double>(diffCount))<<" nanoseconds\n\n"<<std::endl;

    return EXIT_SUCCESS;
}

void fiberFunction(FifoType& recvBuff, FifoType& sendBuff)
{   
    for(unsigned long long i=0; i<iterCount; ++i)
    {
        std::string value;
        clock_gettime(clockType, &stop);
        diffSum += timeDiff(start, stop);
        recvBuff.pop( value);
        clock_gettime(clockType, &start);
        clock_gettime(clockType, &stop);
        callTime += timeDiff(start, stop);
        ++diffCount;
        //std::cout << "fiber " <<  id << ": clong received: " << value << std::endl;
        value.clear();

        sendBuff.push("");
        clock_gettime(clockType, &start);
        boost::this_fiber::yield();
    }

    sendBuff.close();
}

unsigned long long timeDiff(struct timespec start, struct timespec stop)
{
    return ((1000000000ULL*(stop.tv_sec - start.tv_sec)) + (stop.tv_nsec-start.tv_nsec));
}

void printHelp(const std::string& call)
{
    std::cout<<"Usage:\n    " << call << " [ <Num Fibers To Use> [Num Iterations To Run] ]\n"<<std::endl;
    std::cout<<"  <Num Fibers To Use> must be in the range [2, 1000] (inclusive) - defaults to 50"<<std::endl;
    std::cout<<"  [Num Iterations To Run] must be in the range [3, 500000000] (inclusive) - defaults to 1000000"<<std::endl;
}

int checkCmdLineArgs(int argc, char* argv[], int& fiberCount, unsigned long long& iterationCount)
{
    if(argc == 1) // left both to defaults
    {
        iterationCount = 1000000;
        fiberCount = 50;
    }
    else if(argc == 2) // specified a fiber count but not an iteration count
    {
        iterationCount = 1000000;
        std::istringstream conv;
        conv.str(argv[1]);
        conv >> fiberCount;
        if((fiberCount <2) || (fiberCount > 100))
        {
            printHelp(argv[0]);
            return 1;
        }
    }
    else if(argc == 3) // specified both a fiber and iteration count
    {
        std::istringstream conv;
        conv.str(argv[1]);
        conv >> fiberCount;
        if((fiberCount < 2) || (fiberCount > 100))
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
