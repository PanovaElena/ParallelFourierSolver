#include <string>
#include <iostream>
#include "running_wave.h"
#include <string>
#include "fourier_transformation.h"

static double LAMBDA = 2;
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
                if(argc > 4)
                    N_STEPS_IN_PERIOD = atof(argv[4]);
            }
        }
    }
}

void WriteLegend(RunningWave& runningWave) {
    std::cout << "T=" << (runningWave.lambda / constants::c) << std::endl;
    std::cout << "dt=" << runningWave.dt << ", max is " <<
        COURANT_CONDITION_PSTD(runningWave.d) << std::endl;
    std::cout << "n=" << runningWave.nNextSteps << std::endl;
}

void TestBody(RunningWave& runningWave) {
    WriteLegend(runningWave);

    FourierTransformation(runningWave.gr, RtoC);
    for (int j = 1; j <= runningWave.nStartSteps + runningWave.nNextSteps; j++) {
        /*FourierTransformation(runningWave.gr, CtoR);
        runningWave.fileWriter.WriteFile(runningWave.gr, "/consistent_results/cons_res_"+
            std::to_string(j-1)+".csv");*/
        runningWave.fieldSolver(runningWave.gr, runningWave.dt);
    }

    FourierTransformation(runningWave.gr, CtoR);
    runningWave.fileWriter.WriteFile(runningWave.gr, "/consistent_results/E/cons_res.csv", "writing...");
}

int main(int argc, char** argv) {
    RunningWave runningWave;
    CheckArgv(argc, argv, runningWave);
    runningWave.SetParamsForTest(LAMBDA, FIELD_SOLVER, N_PERIODS, N_STEPS_IN_PERIOD);
    TestBody(runningWave);
}