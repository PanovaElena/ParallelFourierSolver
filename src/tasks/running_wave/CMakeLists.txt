add_subdirectory( running_wave_sequential )
add_subdirectory( running_wave_parallel )
add_subdirectory( running_wave_just_parallel )

if(USE_MPI_FFT)
    add_subdirectory( running_wave_mpi_fftw )
endif()