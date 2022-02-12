cmake_minimum_required(VERSION 3.10)
project(nonlinearLib LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(Armadillo CONFIG REQUIRED)

add_library(nonlinear 
    nonlinear.cpp nonlinear.hpp
    analysis.cpp analysis.hpp
    shooting.cpp shooting.hpp
    utils.cpp utils.hpp
    )

target_include_directories(nonlinear PUBLIC ${ARMADILLO_INCLUDE_DIR})
target_link_libraries(nonlinear PUBLIC ${ARMADILLO_LIBRARIES})
