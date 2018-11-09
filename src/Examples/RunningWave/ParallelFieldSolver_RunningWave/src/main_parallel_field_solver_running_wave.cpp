#include "mpi_wrapper.h"
#include "test_parallel.h"
#include "test_running_wave_parallel.h"
#include <string>
#include <iostream>
#include "parser_running_wave.h"
#include "status.h"

int main(int argc, char** argv) {
    MPIWrapper::MPIInitialize(argc, argv);
    MPIWrapper3d mpiWrapper;
    ParametersForRunningWave params;
    ParserRunningWave parser;
    Status status = parser.parseArgsForParallel(argc, argv, params, mpiWrapper);
    if (status == Status::OK) {
        if (MPIWrapper::MPIRank() == 0) params.print();
        TestRunningWaveParallel test(mpiWrapper);
        test.SetParamsForTest(params);
        test.TestBody();
    }
    else if (status == Status::ERROR) std::cout << "There are some problems in args" << std::endl;
    MPIWrapper::MPIFinalize();
}