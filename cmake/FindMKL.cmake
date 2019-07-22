#  Find the Math Kernel Library from Intel
#
#  MKLROOT is to be defined
#
#  MKL_FOUND - System has MKL
#  MKL_INCLUDE_DIRS - MKL include files directories
#  MKL_LIBRARIES - MKL libraries
#
#  The environment variables INTEL are used to find the library.
#
#  Example usage:
#
#  find_package(MKL)
#  if(MKL_FOUND)
#    target_link_libraries(TARGET ${MKL_LIBRARIES})
#  endif()

# If already in cache, be silent
if ( MKL_INCLUDE_DIRS AND MKL_LIBRARIES )
    set (MKL_FIND_QUIETLY TRUE)
endif()

set( MKLROOT $ENV{MKLROOT} )
if( NOT MKLROOT )
    message( WARNING "Environment variable MKLROOT is not defined!" )
endif()
    
find_path( MKL_INCLUDE_DIR NAMES mkl.h HINTS ${MKLROOT}/include)
set( MKL_INCLUDE_DIRS ${MKL_INCLUDE_DIR} ${MKL_INCLUDE_DIR}/fftw )	
set( MKL_LIBRARIES_PATHS "${MKLROOT}/lib;${MKLROOT}/lib/intel64;${INTEL}/mkl/lib/intel64" )

find_library( MKL_CDFT_LIB mkl_cdft_core ${MKL_LIBRARIES_PATHS} )
find_library( MKL_INT_LIB mkl_intel_ilp64 ${MKL_LIBRARIES_PATHS} )
find_library( MKL_INT_THR_LIB mkl_intel_thread ${MKL_LIBRARIES_PATHS} )
find_library( MKL_CORE_LIB mkl_core ${MKL_LIBRARIES_PATHS} )
find_library( MKL_BLACS_INTMPI_LIB mkl_blacs_intelmpi_ilp64 ${MKL_LIBRARIES_PATHS} )


if ( MKL_INCLUDE_DIRS AND
     MKL_CDFT_LIB AND
     MKL_INT_LIB AND
	 MKL_INT_THR_LIB AND 
	 MKL_CORE_LIB AND
	 MKL_BLACS_INTMPI_LIB )
	 
	set( MKL_LIBRARIES "${MKL_CDFT_LIB};${MKL_INT_LIB};${MKL_INT_THR_LIB};${MKL_CORE_LIB};${MKL_BLACS_INTMPI_LIB}" )
	
	if ( WIN32 )
	    find_library( IOMP5_LIB libiomp5md${CMAKE_STATIC_LIBRARY_SUFFIX} $ENV{INTEL}/compiler/lib/intel64 )
		if ( IOMP5_LIB )
		    set(MKL_LIBRARIES "${MKL_LIBRARIES};${IOMP5_LIB}")
		else()
		    message( WARNING "Cannot find iomp5" )
		endif()
	elseif( UNIX )
	    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -liomp5 -lpthread -lm -ldl")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -liomp5 -lpthread -lm -ldl")
	endif()
	 
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMKL_ILP64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMKL_ILP64")
       
    # Handle the QUIETLY and REQUIRED arguments and set MKL_FOUND to TRUE if
    # all listed variables are TRUE.
    INCLUDE(FindPackageHandleStandardArgs)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(MKL DEFAULT_MSG MKL_LIBRARIES MKL_INCLUDE_DIRS)
    
    MARK_AS_ADVANCED(MKL_INCLUDE_DIRS MKL_LIBRARIES)
	
else()
    message( WARNING "Cannot find some necessary mkl components" )
	message( STATUS "MKL_INCLUDE_DIRS=${MKL_INCLUDE_DIRS}")
	message( STATUS "MKL_CDFT_LIB=${MKL_CDFT_LIB}")
	message( STATUS "MKL_INT_LIB=${MKL_INT_LIB}")
	message( STATUS "MKL_INT_THR_LIB=${MKL_INT_THR_LIB}")
	message( STATUS "MKL_CORE_LIB=${MKL_CORE_LIB}")
	message( STATUS "MKL_BLACS_INTMPI_LIB=${MKL_BLACS_INTMPI_LIB}")
endif()
