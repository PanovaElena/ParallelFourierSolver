cmake_minimum_required(VERSION 3.1)

set(PROJECT "ParallelFourierSolvers")

project(${PROJECT})

include(cmake/function.cmake)

add_definitions( -DROOT_DIR="${CMAKE_SOURCE_DIR}" )

set (CMAKE_CXX_STANDARD 11)

if (DCMAKE_CXX_COMPILER MATCHES CMAKE_CXX_COMPILER-NOTFOUND)
    message(WARNING "Cannot find ${DCMAKE_CXX_COMPILER} compiler")
endif()
message(STATUS "Using compiler: ${CMAKE_CXX_COMPILER_ID}")

# libs
set(LIBS_DIR ${CMAKE_SOURCE_DIR}/lib)

# fftw
set(FFTW_DIR ${CMAKE_SOURCE_DIR}/lib/fftw-3.3.5-dll64)
include_directories(${FFTW_DIR})
find_library(FFTW_LIB libfftw3-3 ${FFTW_DIR})

# mpi
if (WIN32)
    set(MPI_LIBRARIES "C:/Program Files (x86)/IntelSWTools/compilers_and_libraries/windows/mpi/intel64/lib")
    set(MPI_INCLUDE_PATH "C:/Program Files (x86)/IntelSWTools/compilers_and_libraries/windows/mpi/intel64/include")
    message(STATUS "MPI_INCLUDE_PATH=${MPI_INCLUDE_PATH}")
else()
    find_package(MPI)
    if (MPI_FOUND)
        message(STATUS "Using MPI  ${MPI_LIBRARIES}")
    else()
        message(WARNING "MPI not found")
    endif()
endif()
include_directories(${MPI_INCLUDE_PATH})

add_element_to_global_property(ALL_LIBS "${FFTW_LIB};${MPI_LIBRARIES}/*.lib")

add_subdirectory(src)
add_subdirectory(test)
