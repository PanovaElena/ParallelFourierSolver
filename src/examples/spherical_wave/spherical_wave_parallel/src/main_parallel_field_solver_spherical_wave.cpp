#include "mpi_wrapper.h"
#include "parser_spherical_wave.h"
#include "test_parallel.h"
#include "test_spherical_wave_parallel.h"
#include <string>
#include <iostream>
#include "status.h"
#include "fftw3.h"

int main(int argc, char** argv) {
    MPIWrapper::MPIInitialize(argc, argv);
    fftw_init_threads();
    ParametersForSphericalWave params;
    ParserSphericalWave parser;
    MPIWorker* mpiWorker = 0;
    int status = parser.parseArgsForParallel(argc, argv, params);
    Status status1 = parser.createMPIWorker(mpiWorker);
    if (status == Status::OK && status1 == Status::OK) {
        if (MPIWrapper::MPIRank() == 0) params.print();
        TestSphericalWaveParallel test(*mpiWorker);
        test.SetParamsForTest(params);
        test.TestBody();
    }
    else if (status == Status::ERROR || status1 == Status::ERROR)
        std::cout << "There are some problems in args" << std::endl;
    delete mpiWorker;
    MPIWrapper::MPIFinalize();
}