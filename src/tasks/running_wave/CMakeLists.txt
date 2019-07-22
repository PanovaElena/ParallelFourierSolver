get_property( PROP GLOBAL PROPERTY TASKS_INCLUDE )
add_element_to_global_property( RUNNING_WAVE_INCLUDE "${PROP};${CMAKE_CURRENT_SOURCE_DIR}/include" )

add_subdirectory( running_wave_sequential )
add_subdirectory( running_wave_parallel )
add_subdirectory( running_wave_just_parallel )

if(USE_FFT_MPI)
    add_subdirectory( running_wave_mpi_fftw )
endif()