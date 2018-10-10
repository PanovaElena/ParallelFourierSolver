#include "mpi_wrapper.h"
#include "parser_spherical_wave.h"
#include "test_parallel.h"
#include "test_spherical_wave_parallel.h"
#include <string>
#include <iostream>

int main(int argc, char** argv) {
    MPIWrapper::MPIInitialize();
    TestSphericalWaveParallel test;
    ParametersForSphericalWave params;
    ParserSphericalWave parser;
    int status = parser.parseArgs(argc, argv, params);
    if (status == 0) {
        if (MPIWrapper::MPIRank() == 0) params.print();
        test.sphericalWave.SetParamsForTest(params);
        test.TestBody();
    }
    MPIWrapper::MPIFinalize();
}