cmake_minimum_required(VERSION 3.10)
project(nonlinearLib LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(Armadillo_DIR "D:/vcpkg/installed/x64-mingw-static/share/Armadillo")
find_package(Armadillo CONFIG REQUIRED)

find_package(Python COMPONENTS Development REQUIRED)

add_library(nonlinear 
    nonlinear.cpp   nonlinear.hpp
    utils.cpp       utils.hpp
    pynl.cpp        pynl.hpp
    )
target_include_directories(nonlinear PUBLIC ${ARMADILLO_INCLUDE_DIRS} ${Python_INCLUDE_DIRS})
target_link_libraries(nonlinear PUBLIC ${ARMADILLO_LIBRARIES} ${Python_LIBRARIES})
