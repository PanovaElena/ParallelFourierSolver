cmake_minimum_required(VERSION 2.8.11)

set(PROJECT "ParallelFourierSolvers")

project(${PROJECT})

include(cmake/function.cmake)

add_definitions( -DROOT_DIR="${CMAKE_SOURCE_DIR}" )

if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
    else()
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif ()
else ()
    set (CMAKE_CXX_STANDARD 11)
endif ()


if (DCMAKE_CXX_COMPILER MATCHES CMAKE_CXX_COMPILER-NOTFOUND)
    message(WARNING "Cannot find ${DCMAKE_CXX_COMPILER} compiler")
endif()
message(STATUS "Using compiler: ${CMAKE_CXX_COMPILER_ID}")

# mpi
if (WIN32)
    set(MPI_LIBRARIES "C:/Program Files (x86)/IntelSWTools/compilers_and_libraries/windows/mpi/intel64/lib" CACHE STRING "Path to MPI libraries")
    set(MPI_INCLUDE_PATH "C:/Program Files (x86)/IntelSWTools/compilers_and_libraries/windows/mpi/intel64/include" CACHE PATH "Path to MPI includes")
    add_element_to_global_property(ALL_LIBS "${MPI_LIBRARIES}/*.lib;")
else()
    find_package(MPI)
    if (MPI_FOUND)
        message(STATUS "MPI was found successfully")
    else()
        set(MPI_LIBRARIES "/common/intel/impi/4.1.3.048/intel64/lib" CACHE STRING "Path to MPI libraries")
        set(MPI_INCLUDE_PATH "/common/intel/impi/4.1.3.048/intel64/include" CACHE PATH "Path to MPI includes")
    endif()
    find_library(MPI_LIB mpi ${MPI_LIBRARIES}) 
    add_element_to_global_property(ALL_LIBS "${MPI_LIB};")
endif()
message(STATUS "MPI: ${MPI_LIBRARIES}")
include_directories(${MPI_INCLUDE_PATH})

# fftw
if (WIN32)
    set(FFTW_DIR ${CMAKE_SOURCE_DIR}/lib/fftw-3.3.5_windows_x64)
    include_directories(${FFTW_DIR})
    find_library(FFTW_LIB libfftw3-3 ${FFTW_DIR})
else()
    set(FFTW_DIR ${CMAKE_SOURCE_DIR}/lib/fftw-3.3.8_unix)
    include_directories(${FFTW_DIR}/include)
    find_library(FFTW_LIB fftw3 ${FFTW_DIR}/lib)
endif()
message(STATUS "FFTW: ${FFTW_LIB}")
add_element_to_global_property(ALL_LIBS ${FFTW_LIB})

add_subdirectory(src)
add_subdirectory(test)

get_property(PROP GLOBAL PROPERTY ALL_LIBS)
message("LIST=${PROP}")
