cmake_minimum_required(VERSION 3.22)
project(osex4 C)

set(CMAKE_C_STANDARD 99)

add_executable(osex4 ex4_srv.c ex4_client.c)
