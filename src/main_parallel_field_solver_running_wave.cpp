#include "test_running_wave_parallel.h"
#include "test_parallel.h"
#include "mpi_wrapper.h"
#include <string>

int main(int* argc, char** argv) {
    MPIWrapper::MPIInitialize();
    TestRunningWaveParallel test;
    test.TestBody();
    MPIWrapper::MPIFinalize();
}