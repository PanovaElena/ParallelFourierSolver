#include <string>
#include <iostream>
#include "spherical_wave.h"
#include <string>
#include "fourier_transformation.h"

static FieldSolverType FIELD_SOLVER = FieldSolverPSATD;
static int N_STEPS = 400;

void CheckArgv(int argc, char** argv, SphericalWave& sphericalWave) {
    if (argc > 1) {
        if (std::string(argv[1]) == "PSTD")
            FIELD_SOLVER = FieldSolverPSTD;
        else FIELD_SOLVER = FieldSolverPSATD;
        if (argc > 2)
            N_STEPS = atoi(argv[2]);
    }
}

void TestBody(SphericalWave& sphericalWave) {
    FourierTransformation(sphericalWave.gr, RtoC);
    for (int j = 1; j <= sphericalWave.nNextSteps; j++) {
        /*FourierTransformation(sphericalWave.gr, CtoR);
        sphericalWave.fileWriter.WriteFile(sphericalWave.gr, "/consistent_results/cons_res_"+
        std::to_string(j-1)+".csv");*/
        sphericalWave.SetJ(j);
        sphericalWave.fieldSolver(sphericalWave.gr, sphericalWave.dt);
    }

    FourierTransformation(sphericalWave.gr, CtoR);
    sphericalWave.fileWriter.WriteFile(sphericalWave.gr, "/consistent_results/E/cons_res.csv", "writing...");
}

int main(int argc, char** argv) {
    SphericalWave sphericalWave;
    CheckArgv(argc, argv, sphericalWave);
    sphericalWave.SetParamsForTest(FIELD_SOLVER, 0, N_STEPS);
    TestBody(sphericalWave);
}