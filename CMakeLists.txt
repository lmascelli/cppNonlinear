cmake_minimum_required(VERSION 3.10)
project(nonlinear LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

find_package(Python COMPONENTS Development REQUIRED)
add_subdirectory(cppLib)

add_library(system_test SHARED
            test/system_test.cpp)
target_include_directories(system_test PUBLIC ${Python_INCLUDE_DIRS} cppLib/include)
target_link_libraries(system_test PUBLIC nonlinear ${Python_LIBRARIES})