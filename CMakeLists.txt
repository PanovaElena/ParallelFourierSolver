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
	include_directories(${MPI_INCLUDE_PATH})
else()
    find_package(MPI)
    if (MPI_FOUND)
        message(STATUS "MPI: success")
    else()
        set(MPI_LIBRARIES "/common/intel/impi/4.1.3.048/intel64/lib" CACHE STRING "Path to MPI libraries")
        set(MPI_INCLUDE_PATH "/common/intel/impi/4.1.3.048/intel64/include" CACHE PATH "Path to MPI includes")
    endif()
    find_library(MPI_LIB mpi ${MPI_LIBRARIES}) 
    add_element_to_global_property(ALL_LIBS "${MPI_LIB};")
	include_directories(${MPI_INCLUDE_PATH})
endif()
message(STATUS "MPI: ${MPI_LIBRARIES}")

#OpenMP
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    message(STATUS "OpenMP: success")
else()
    message(WARNING "Cannot find OpenMP")
endif()

# fftw
if (WIN32)
    set(FFTW_DIR ${CMAKE_SOURCE_DIR}/lib/fftw-3.3.5_windows_x64)
    include_directories(${FFTW_DIR})
    find_library(FFTW_LIB libfftw3-3 ${FFTW_DIR})
    message(STATUS "FFTW: ${FFTW_LIB}")
    add_element_to_global_property(ALL_LIBS ${FFTW_LIB})
else()
    ## Fix for FFTW
    include(ExternalProject)
    MESSAGE(STATUS "Trying to install fftw...")
    ExternalProject_Add(project_fftw
      #GIT_REPOSITORY  https://github.com/FFTW/fftw3
      URL http://www.fftw.org/fftw-3.3.8.tar.gz
      PREFIX ${CMAKE_CURRENT_BINARY_DIR}/fftw
      CONFIGURE_COMMAND ${CMAKE_CURRENT_BINARY_DIR}/fftw/src/project_fftw/configure --enable-openmp --prefix=${CMAKE_CURRENT_BINARY_DIR}/fftw/install
      INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/fftw/install
    )
    add_library(fftw STATIC IMPORTED)
    set(lib_fftw_name ${CMAKE_STATIC_LIBRARY_PREFIX}fftw3${CMAKE_STATIC_LIBRARY_SUFFIX})
    set_target_properties(fftw PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/fftw/install/lib/${lib_fftw_name})
    add_library(fftw_omp STATIC IMPORTED)
    set(lib_fftw_omp_name ${CMAKE_STATIC_LIBRARY_PREFIX}fftw3_omp${CMAKE_STATIC_LIBRARY_SUFFIX})
    set_target_properties(fftw_omp PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/fftw/install/lib/${lib_fftw_omp_name})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/fftw/install/include)
	add_element_to_global_property(ALL_LIBS fftw3)
	add_element_to_global_property(ALL_LIBS fftw3_omp)
    ## End Fix for FFTW
endif()

add_subdirectory(src)
add_subdirectory(test)
