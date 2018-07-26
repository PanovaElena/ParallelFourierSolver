#include "test_parallel.h"
#include "test_standing_wave_parallel.h"
#include "mpi_wrapper.h"
#include <string>

int main(int* argc, char** argv) {
    MPIWrapper::MPIInitialize();
    TestStandingWaveParallel test;
    test.TestBody();
    MPIWrapper::MPIFinalize();
}