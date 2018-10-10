#include "mpi_wrapper.h"
#include "test_parallel.h"
#include "test_running_wave_parallel.h"
#include <string>
#include <iostream>
#include "parser_running_wave.h"

int main(int argc, char** argv) {
    MPIWrapper::MPIInitialize();
    TestRunningWaveParallel test;
    ParametersForRunningWave params;
    ParserRunningWave parser;
    int status = parser.parseArgs(argc, argv, params);
    if (status == 0) {
        if (MPIWrapper::MPIRank() == 0) params.print();
        test.runningWave.SetParamsForTest(params);
        test.TestBody();
    }
    MPIWrapper::MPIFinalize();
}