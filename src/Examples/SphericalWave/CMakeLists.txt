include_directories(
    ${MPI_DIR}
    ../include
    include
    src/ConsistentFieldSolver_SphericalWave
    src/ParallelFieldSolver_SphericalWave
    ../../FourierFieldSolver/include)

add_executable(ParallelFieldSolver_SphericalWave
    ${MPI_DIR}/mpi.h
    ../include/test_parallel.h
    include/spherical_wave.h
    include/test_spherical_wave_parallel.h

    src/ParallelFieldSolver_SphericalWave/main_parallel_field_solver_spherical_wave.cpp)

target_link_libraries(ParallelFieldSolver_SphericalWave 
                                                        ${FFTW_LIB}
                                                        ${MPI_LIB}
                                                        FourierFieldSolver)

add_executable(ConsistentFieldSolver_SphericalWave
    include/spherical_wave.h

    src/ConsistentFieldSolver_SphericalWave/main_consistent_field_solver_spherical_wave.cpp)

target_link_libraries(ConsistentFieldSolver_SphericalWave 
                                                        ${FFTW_LIB}
                                                        FourierFieldSolver)