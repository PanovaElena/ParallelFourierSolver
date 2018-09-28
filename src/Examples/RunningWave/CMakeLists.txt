include_directories(
    ${MPI_DIR}
    ../include
    include
    src/ConsistentFieldSolver_RunningWave
    src/ParallelFieldSolver_RunningWave
    ../../FourierFieldSolver/include)

add_executable(ParallelFieldSolver_RunningWave
    ${MPI_DIR}/mpi.h
    ../include/test_parallel.h
    include/running_wave.h
    include/test_running_wave_parallel.h

    src/ParallelFieldSolver_RunningWave/main_parallel_field_solver_running_wave.cpp)

target_link_libraries(ParallelFieldSolver_RunningWave
                                                        ${FFTW_LIB}
                                                        ${MPI_LIB}
                                                        FourierFieldSolver)

add_executable(ConsistentFieldSolver_RunningWave
    include/running_wave.h

    src/ConsistentFieldSolver_RunningWave/main_consistent_field_solver_running_wave.cpp)

target_link_libraries(ConsistentFieldSolver_RunningWave 
                                                        ${FFTW_LIB}
                                                        FourierFieldSolver)