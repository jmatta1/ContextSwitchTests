#include<time.h>
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<array>
#include<x86intrin.h>
#include<boost/bind.hpp>
#include<boost/fiber/all.hpp>

typedef boost::fibers::fiber FiberType;

static const int SelectedPmc = ((1<<30)+2);

//timing variables
long long start, stop;
long long diffSum=0ULL;
long long callTime=0ULL;
long long diffCount=0;
//iteration limit (set elsewhere)
long long iterCount;

int checkCmdLineArgs(int argc, char* argv[], int& fiberCount, long long& iterationCount);
void fiberFunction();

int main(int argc, char* argv[])
{
    std::cout << start << ", " << stop << std::endl;
    int fiberCount;
    if(checkCmdLineArgs(argc, argv, fiberCount, iterCount) != 0)
    {
        return 1;
    }
    
    // set up the fibers
    int lastIndex = (fiberCount - 1);

    // generate the set of fibers
    std::vector<FiberType> fiberVector;
    for(int i=0; i<lastIndex; ++i)
    {
        fiberVector.emplace_back(FiberType(boost::bind(&fiberFunction)));
    }

    //execute the main fiber
    for(unsigned long long i=0; i<iterCount; ++i)
    {
        start = __rdtsc();//__rdpmc(SelectedPmc);
        boost::this_fiber::yield();

        stop = __rdtsc();//__rdpmc(SelectedPmc);
        diffSum += (stop - start);
        start = __rdtsc();//__rdpmc(SelectedPmc);
        stop = __rdtsc();//__rdpmc(SelectedPmc);
        callTime += (stop - start);
        ++diffCount;
    }

    for(int i=0; i<lastIndex; ++i)
    {
        fiberVector[i].join();
    }

    double avgTimeDiff = (static_cast<double>(diffSum) / static_cast<double>(diffCount));
    double avgCallTime = (static_cast<double>(callTime) / static_cast<double>(diffCount));

    std::cout << "\n\nFiber Context Switch Test Complete." << "  Ran With "
              << fiberCount << " Fibers and " << iterCount << " Iterations"
              << std::endl;

    std::cout << "    Total Fiber Context Switch Time Difference Was: " << diffSum << " cycles across " << diffCount <<
                 " Switches.\n\tAverage Time Difference Was: "
              << avgTimeDiff <<" cycles"<<std::endl;

    std::cout << "    Total Call rdpmc_actual_cycles Time Difference Was: " << callTime << " cycles across " << diffCount <<
                 " Call Pairs.\n\tAverage Call rdpmc_actual_cycles Time Difference Was: "
              << avgCallTime <<" cycles"<<std::endl;

    std::cout << "\nThe average, rdpmc_actual_cycles call compensated, fiber context switch time is: " <<
                  (avgTimeDiff - avgCallTime) <<" cycles\n\n"<<std::endl;

    return 0;
}

void fiberFunction()
{   
    for(unsigned long long i=0; i<iterCount; ++i)
    {
        stop = __rdtsc();//__rdpmc(SelectedPmc);
        diffSum += (stop - start);
        start = __rdtsc();//__rdpmc(SelectedPmc);
        stop = __rdtsc();//__rdpmc(SelectedPmc);
        callTime += (stop - start);
        ++diffCount;
        start = __rdtsc();//__rdpmc(SelectedPmc);
        boost::this_fiber::yield();
    }
}

void printHelp(const std::string& call)
{
    std::cout<<"Usage:\n    " << call << " [ <Num Fibers To Use> [Num Iterations To Run] ]\n"<<std::endl;
    std::cout<<"  <Num Fibers To Use> must be in the range [2, 1000] (inclusive) - defaults to 50"<<std::endl;
    std::cout<<"  [Num Iterations To Run] must be in the range [3, 500000000] (inclusive) - defaults to 1000000"<<std::endl;
}

int checkCmdLineArgs(int argc, char* argv[], int& fiberCount, long long& iterationCount)
{
    if(argc == 1) // left both to defaults
    {
        iterationCount = 1000000;
        fiberCount = 4;
    }
    else if(argc == 2) // specified a fiber count but not an iteration count
    {
        iterationCount = 1000000;
        std::istringstream conv;
        conv.str(argv[1]);
        conv >> fiberCount;
        if((fiberCount <2) || (fiberCount > 1000))
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
        if((fiberCount < 2) || (fiberCount > 1000))
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
