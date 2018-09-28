include_directories(
    ${FFTW_DIR}
    ${MPI_DIR}
    ../gtest
    ${ROOT_DIR}/src/FourierFieldSolver/include
    ${ROOT_DIR}/src/Examples/RunningWave/include
    ${ROOT_DIR}/src/Examples/SphericalWave/include)

add_executable(Tests
    field_solver_for_testing.h
    ../gtest/gtest.h

    test_field_solver_running_wave.cpp
    test_field_solver_spherical_wave.cpp
    test_fourier_transformation.cpp
    test_grid3d.cpp
    test_lag.cpp
    test_main.cpp
    test_mpi_worker.cpp
    test_my_complex.cpp
    test_vec.cpp)

target_link_libraries(Tests 
                            ${FFTW_LIB}
                            gtest
                            FourierFieldSolver)