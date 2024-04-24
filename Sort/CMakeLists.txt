cmake_minimum_required(VERSION 3.27)

project(Sort)

find_package(MPI REQUIRED)

set(CMAKE_C_COMPILER mpicc)
set(CMAKE_CXX_COMPILER mpicxx)

include_directories(${MPI_INCLUDE_PATH})

set(SOURCES 
    src/main.cpp
    src/sorting_network.cpp
    src/parallel_sort.cpp
)

add_executable(main ${SOURCES})

target_link_libraries(main ${MPI_CXX_LIBRARIES})