cmake_minimum_required(VERSION 3.27)

project(Exponent)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})
set(GMP_LIBRARIES "/usr/lib/x86_64-linux-gnu/libgmp.so")

add_executable(main main.cpp)
target_link_libraries(main ${MPI_mpi_LIBRARY} ${MPI_LIBRARIES} ${GMP_LIBRARIES})

if(MPI_COMPILE_FLAGS)
  set_target_properties(main PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(main PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()