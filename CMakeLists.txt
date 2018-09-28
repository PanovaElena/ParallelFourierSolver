cmake_minimum_required(VERSION 2.8.11)

set(PROJECT "ParallelFourierSolvers")

project(${PROJECT})

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(ARCH "x64")

if (DCMAKE_CXX_COMPILER MATCHES CMAKE_CXX_COMPILER-NOTFOUND)
    message(WARNING "Cannot find ${DCMAKE_CXX_COMPILER} compiler")
endif()
message(STATUS "Using compiler: ${CMAKE_CXX_COMPILER_ID}")

# libs
set(LIBS_DIR ${ROOT_DIR}/lib)

# fftw
set(FFTW_DIR ${LIBS_DIR}/fftw-3.3.5-dll64)
find_library(FFTW_LIB libfftw3-3 ${FFTW_DIR})

# mpi
set(MPI_DIR ${LIBS_DIR}/mpi)
find_library(MSMPI_LIB msmpi ${MPI_DIR}/x64)
find_library(MSMPIFEC_LIB msmpifec ${MPI_DIR}/x64/)
find_library(MSMPIFMC_LIB msmpifmc ${MPI_DIR}/x64/)
set(MPI_LIB
            ${MSMPI_LIB}
            ${MSMPIFEC_LIB}
            ${MSMPIFMC_LIB})


add_subdirectory(src)
add_subdirectory(test)
