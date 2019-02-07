cmake_minimum_required( VERSION 2.8.11 )

set( PROJECT "parallel_fourier_solvers" )

project( ${PROJECT} )

include( cmake/function.cmake )

include( ExternalProject )

add_definitions( -DROOT_DIR="${CMAKE_SOURCE_DIR}" )

set( INSTALL_DIR "${CMAKE_BINARY_DIR}/3rdparty" )
file( MAKE_DIRECTORY "${INSTALL_DIR}" )

set( FFTW_VERSION 3.3.8 )


if ( DCMAKE_CXX_COMPILER MATCHES CMAKE_CXX_COMPILER-NOTFOUND )
    message( WARNING "Cannot find ${DCMAKE_CXX_COMPILER} compiler" )
endif()
message( STATUS "Using compiler: ${CMAKE_CXX_COMPILER_ID}" )


if ( CMAKE_VERSION VERSION_LESS "3.1" )
    if ( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11" )
    else()
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
    endif()
else()
    set( CMAKE_CXX_STANDARD 11 )
endif()


if ( MSVC )
    add_definitions( -D_CRT_SECURE_NO_WARNINGS )
    set( CMAKE_EXE_LINKER_FLAGS "/NODEFAULTLIB:libcmt.lib" )
    set( CMAKE_SHARED_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} )
    if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
        set( CMAKE_EXE_LINKER_FLAGS "/machine:x64 ${CMAKE_EXE_LINKER_FLAGS}" )
        set( CMAKE_SHARED_LINKER_FLAGS "/machine:x64 ${CMAKE_EXE_LINKER_FLAGS}" )
        set( CMAKE_MODULE_LINKER_FLAGS "/machine:x64" )
        set( CMAKE_STATIC_LINKER_FLAGS "/machine:x64" )
    endif()
endif()


# mpi
if ( WIN32 )
    set( MPI_LIBRARIES "C:/Program Files (x86)/IntelSWTools/compilers_and_libraries/windows/mpi/intel64/lib" CACHE STRING "Path to MPI libraries" )
    set( MPI_INCLUDE_PATH "C:/Program Files (x86)/IntelSWTools/compilers_and_libraries/windows/mpi/intel64/include" CACHE PATH "Path to MPI includes" )
	include_directories( ${MPI_INCLUDE_PATH} )
    add_element_to_global_property( ALL_LIBS ${MPI_LIBRARIES}/*.lib )
else()
    find_package( MPI )
	if ( MPI_FOUND )
        find_library( MPI_LIB mpi ${MPI_LIBRARIES} ) 
        add_element_to_global_property( ALL_LIBS ${MPI_LIB} )
        include_directories( ${MPI_INCLUDE_PATH} )
	else()
	    message( WARNING "Cannot find MPI" )
	endif()
endif()


# OpenMP
find_package( OpenMP )
if ( OPENMP_FOUND )
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
    set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}" )
else()
    message( WARNING "Cannot find OpenMP" )
endif()


# fftw
ExternalProject_Add( project_fftw
    URL "http://fftw.org/fftw-${FFTW_VERSION}.tar.gz"
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/fftw
    CMAKE_ARGS
        "-DCMAKE_CFLAGS=${CMAKE_C_FLAGS}"
        "-DBUILD_SHARED_LIBS=OFF"
        "-DENABLE_AVX2=ON"
        "-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/fftw" 
		"-DENABLE_OPENMP=ON"
		"-DBUILD_TESTS=OFF"
		)

install( DIRECTORY "${INSTALL_DIR}" DESTINATION . )

set( FFTW_DIR ${INSTALL_DIR}/fftw )
include_directories( ${FFTW_DIR}/include )
add_element_to_global_property( ALL_LIBS ${FFTW_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}fftw3${CMAKE_STATIC_LIBRARY_SUFFIX} )
add_element_to_global_property( ALL_LIBS ${FFTW_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}fftw3_omp${CMAKE_STATIC_LIBRARY_SUFFIX} )


add_subdirectory( src )
add_subdirectory( test )
