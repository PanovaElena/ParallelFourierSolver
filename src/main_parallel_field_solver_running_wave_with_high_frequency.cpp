#include "test_running_wave_with_high_frequency_parallel.h"
#include "test_parallel.h"
#include "mpi_wrapper.h"
#include <string>

int main(int* argc, char** argv) {
    MPIWrapper::MPIInitialize();
    TestRunningWaveWithHighFrequencyParallel test;
    test.TestBody();
    MPIWrapper::MPIFinalize();
}