cmake_minimum_required(VERSION 3.1)

###########################################################
##  Setup Project
###########################################################
project(TestFibers)

###########################################################
##  Setup C++ options
###########################################################
# set for c++ 11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

###########################################################
##  Setup External Library Dependencies
###########################################################
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

###########################################################
##  Setup Build Options
###########################################################
include(./BldOpt.cmake)


###########################################################
##  Grab all the sub-directory file data
###########################################################
list(APPEND SRCS ./main.cpp)

###########################################################
##  Target Setup
###########################################################
# Add the executable and the appropriate link
add_executable(testThreads ${SRCS} ${HDRS})
target_link_libraries(testThreads ${Boost_LIBRARIES})
