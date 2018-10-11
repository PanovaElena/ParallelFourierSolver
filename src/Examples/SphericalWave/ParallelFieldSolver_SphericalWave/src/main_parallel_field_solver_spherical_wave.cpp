#include "mpi_wrapper.h"
#include "parser_spherical_wave.h"
#include "test_parallel.h"
#include "test_spherical_wave_parallel.h"
#include <string>
#include <iostream>

int main(int argc, char** argv) {
    MPIWrapper::MPIInitialize();
    MPIWrapper3d mpiWrapper;
    ParametersForSphericalWave params;
    ParserSphericalWave parser;
    int status = parser.parseArgsForParallel(argc, argv, params, mpiWrapper);
    if (status == 0) {
        if (MPIWrapper::MPIRank() == 0) params.print();
        TestSphericalWaveParallel test(mpiWrapper);
        test.sphericalWave.SetParamsForTest(params);
        test.TestBody();
    }
    else if (status == 1) std::cout << "There are some problems in args" << std::endl;
    MPIWrapper::MPIFinalize();
}