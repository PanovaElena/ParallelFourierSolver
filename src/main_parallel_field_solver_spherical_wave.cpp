#include "test_parallel.h"
#include "test_spherical_wave_parallel.h"
#include "mpi_wrapper.h"
#include <string>

static FieldSolverType FIELD_SOLVER = FieldSolverPSATD;
static int N_CONS_STEPS = 300;
static int N_PAR_STEPS = 100;

void CheckArgv(int argc, char** argv, SphericalWave& sphericalWave) {
    if (argc > 1) {
        if (std::string(argv[1]) == "PSTD")
            FIELD_SOLVER = FieldSolverPSTD;
        else FIELD_SOLVER = FieldSolverPSATD;
        if (argc > 2) {
            N_CONS_STEPS = atoi(argv[2]);
            if (argc > 3)
                N_PAR_STEPS = atoi(argv[3]);
        }
    }
}

int main(int argc, char** argv) {
    MPIWrapper::MPIInitialize();
    TestSphericalWaveParallel test;
    CheckArgv(argc, argv, test.sphericalWave);
    test.sphericalWave.SetParamsForTest(FIELD_SOLVER, N_CONS_STEPS, N_PAR_STEPS);
    test.TestBody();
    MPIWrapper::MPIFinalize();
}