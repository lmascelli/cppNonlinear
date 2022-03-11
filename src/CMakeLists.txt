cmake_minimum_required(VERSION 3.10)
project(nonlinearLib LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_POSITION_INDEPENDENT_CODE True)

# set(Armadillo_DIR "D:/vcpkg/installed/x64-mingw-static/share/Armadillo")
find_package(Armadillo CONFIG REQUIRED)

add_library(nonlinear 
    nonlinear.cpp   nonlinear.hpp
    )
target_include_directories(nonlinear PUBLIC ${ARMADILLO_INCLUDE_DIRS})
target_link_libraries(nonlinear PUBLIC ${ARMADILLO_LIBRARIES})


find_package(Python COMPONENTS Development REQUIRED)
add_library(pynl SHARED
    pynl.cpp        )
target_include_directories(pynl PUBLIC ${Python_INCLUDE_DIRS})
target_link_libraries(pynl PUBLIC nonlinear ${Python_LIBRARIES})