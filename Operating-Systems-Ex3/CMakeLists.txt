cmake_minimum_required(VERSION 3.22)
project(osex3)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS -pthread)
add_executable(osex3 UBQ.cpp UBQ.h BQ.cpp BQ.h main.cpp)
