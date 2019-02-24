cmake_minimum_required( VERSION 2.8.11 )

option( USE_MKL "using mkl" ON )
option( USE_FFTW "using fftw" OFF )

set( PROJECT "parallel_fourier_solvers" )
project( ${PROJECT} )

include( cmake/function.cmake )
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

add_definitions( -DROOT_DIR="${CMAKE_SOURCE_DIR}" )

set( INSTALL_DIR "${CMAKE_BINARY_DIR}/3rdparty" )
file( MAKE_DIRECTORY "${INSTALL_DIR}" )


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
    set( MPI_LIBRARIES "${I_MPI_ROOT}/intel64/lib/*" )
    set( MPI_INCLUDE_PATH "$ENV{I_MPI_ROOT}/intel64/include" )
    include_directories( ${MPI_INCLUDE_PATH} )
    add_element_to_global_property( ALL_LIBS "${MPI_LIBRARIES}" )
else()
    find_package( MPI )
    if ( MPI_FOUND )
        find_library( MPI_LIB mpi ${MPI_LIBRARIES} ) 
        add_element_to_global_property( ALL_LIBS "${MPI_LIB}" )
        include_directories( ${MPI_INCLUDE_PATH} )
    else()
        message( STATUS "Cannot find MPI, I_MPI_ROOT is used" )
        set( I_MPI_ROOT $ENV{I_MPI_ROOT} )
        if ( I_MPI_ROOT )
            set( MPI_LIBRARIES "${I_MPI_ROOT}/intel64/lib" )
            find_library( MPI_LIB mpi "${MPI_LIBRARIES}" ) 
            set( MPI_INCLUDE_PATH "${I_MPI_ROOT}/intel64/include" )
	    include_directories( ${MPI_INCLUDE_PATH} )
            add_element_to_global_property( ALL_LIBS "${MPI_LIB}" )
        else()
            message( WARNING "I_MPI_ROOT is not defined!" )
        endif()
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


# mkl fft
if ( USE_MKL )
    find_package( MKL )
    if ( MKL_FOUND )
        include_directories( ${MKL_INCLUDE_DIRS} )
        add_element_to_global_property( ALL_LIBS "${MKL_LIBRARIES}" )
	else()	
		message( WARNING "Cannot find MKL" )
	endif()
endif()

# fftw
if ( USE_FFTW )
    
    set( FFTW_VERSION 3.3.8 )
    include( ExternalProject )
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
	find_library( FFTW3_LIB ${CMAKE_STATIC_LIBRARY_PREFIX}fftw3${CMAKE_STATIC_LIBRARY_SUFFIX} ${FFTW_DIR}/lib )
	find_library( FFTW3_OMP_LIB ${CMAKE_STATIC_LIBRARY_PREFIX}fftw3_omp${CMAKE_STATIC_LIBRARY_SUFFIX} ${FFTW_DIR}/lib )
    add_element_to_global_property( ALL_LIBS "${FFTW3_LIB};${FFTW3_OMP_LIB}" )
	
endif()


add_subdirectory( src )
add_subdirectory( test )
