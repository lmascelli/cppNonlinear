cmake_minimum_required(VERSION 3.10)
project(nonlinear LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

find_package(Python COMPONENTS Development REQUIRED)
add_subdirectory(src)

add_library(system_test SHARED
            system_test.cpp)
target_include_directories(system_test PUBLIC ${Python_INCLUDE_DIRS})
target_link_libraries(system_test PUBLIC nonlinear ${Python_LIBRARIES})