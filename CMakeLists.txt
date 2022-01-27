cmake_minimum_required(VERSION 3.10)
project(test LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)


add_subdirectory(src)

add_executable(test test.cpp)
target_link_libraries(test PRIVATE nonlinear)