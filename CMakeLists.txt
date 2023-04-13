cmake_minimum_required(VERSION 3.3)
project(parallel-edit-distance)             

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -fopenmp")

set(SOURCE_FILES sequential.cpp, parallel_diag_fronts.cpp, parallel_tiling.cpp)
add_executable(sequential sequential.cpp)
add_executable(parallel_diag_fronts parallel_diag_fronts.cpp)
add_executable(parallel_tiling parallel_tiling.cpp)