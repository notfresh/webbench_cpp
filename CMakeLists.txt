cmake_minimum_required(VERSION 3.10)

project(webbench VERSION 1.5)

set(CMAKE_CXX_STANDARD  11)
set(CMAKE_CXX_STANDARD_REQUIRED  True)

add_executable(build_request.cpp)