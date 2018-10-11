#include "mpi_wrapper.h"
#include "test_parallel.h"
#include "test_running_wave_parallel.h"
#include <string>
#include <iostream>
#include "parser_running_wave.h"

int main(int argc, char** argv) {
    MPIWrapper::MPIInitialize();
    MPIWrapper3d mpiWrapper;
    ParametersForRunningWave params;
    ParserRunningWave parser;
    int status = parser.parseArgsForParallel(argc, argv, params, mpiWrapper);
    if (status == 0) {
        if (MPIWrapper::MPIRank() == 0) params.print();
        TestRunningWaveParallel test(mpiWrapper);
        test.runningWave.SetParamsForTest(params);
        test.TestBody();
    }
    else if (status == 1) std::cout << "There are some problems in args" << std::endl;
    MPIWrapper::MPIFinalize();
}