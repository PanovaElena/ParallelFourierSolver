#include <string>
#include <iostream>
#include "parser_spherical_wave.h"
#include "spherical_wave.h"
#include "fourier_transformation.h"
#include "field_solver.h"


void TestBody(SphericalWave& sphericalWave) {
    TransformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, RtoC);
    for (int j = 1; j <= sphericalWave.parameters.nSeqSteps; j++) {
        TransformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, CtoR);
        sphericalWave.SetJ(j);
        TransformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, RtoC);
        sphericalWave.parameters.fieldSolver(sphericalWave.gr, sphericalWave.parameters.dt);
    }

    TransformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, CtoR);
    sphericalWave.parameters.fileWriter.WriteFile(sphericalWave.gr, "sequential_result.csv", Double, "writing...");
}

int main(int argc, char** argv) {
    SphericalWave sphericalWave;
    ParserSphericalWave parser;
    ParametersForSphericalWave params;
    int status = parser.parseArgsForSequential(argc, argv, params);
    if (status != 0) return 0;
    params.nParSteps = 0;
    params.print();
    sphericalWave.SetParamsForTest(params);
    TestBody(sphericalWave);
}