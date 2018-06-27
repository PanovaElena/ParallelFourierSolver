#include "test_parallel.h"
#include "test_spherical_wave_parallel.h"
#include "mpi_wrapper.h"
#include <string>

int main(int* argc, char** argv) {
    MPIWrapper::MPIInitialize();
    TestSphericalWaveParallel test;
    test.TestBody();
    MPIWrapper::MPIFinalize();
}