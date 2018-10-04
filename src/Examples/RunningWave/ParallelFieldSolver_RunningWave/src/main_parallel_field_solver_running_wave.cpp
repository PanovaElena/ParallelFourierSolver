#include "mpi_wrapper.h"
#include "test_parallel.h"
#include "test_running_wave_parallel.h"
#include <string>
#include <iostream>
#include "field_solver.h"

static double LAMBDA;
static FieldSolverType FIELD_SOLVER = FieldSolverPSATD;
static double N_PERIODS = -1;
static double N_STEPS_IN_PERIOD = -1;

void CheckArgv(int argc, char** argv, RunningWave& runningWave) {
    if (argc > 1) {
        LAMBDA = atoi(argv[1])*runningWave.d;
        if (argc > 2) {
            if (std::string(argv[2]) == "PSTD")
                FIELD_SOLVER = FieldSolverPSTD;
            else FIELD_SOLVER = FieldSolverPSATD;
            if (argc > 3) {
                N_PERIODS = atof(argv[3]);
                if (argc > 4)
                    N_STEPS_IN_PERIOD = atof(argv[4]);
            }
        }
    }
}

int main(int argc, char** argv) {
    MPIWrapper::MPIInitialize();
    TestRunningWaveParallel test;
    CheckArgv(argc, argv, test.runningWave);
    test.runningWave.SetParamsForTest(LAMBDA, FIELD_SOLVER, N_PERIODS, N_STEPS_IN_PERIOD);
    test.TestBody();
    MPIWrapper::MPIFinalize();
}