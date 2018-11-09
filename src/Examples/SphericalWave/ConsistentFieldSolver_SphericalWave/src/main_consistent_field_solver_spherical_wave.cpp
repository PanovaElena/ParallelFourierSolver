#include <string>
#include <iostream>
#include "parser_spherical_wave.h"
#include "spherical_wave.h"
#include "fourier_transformation.h"
#include "field_solver.h"


void TestBody(SphericalWave& sphericalWave) {
    TransformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, RtoC);
    for (int j = 1; j <= sphericalWave.parameters.nConsSteps; j++) {
        /*FourierTransformation(sphericalWave.gr, CtoR);
        sphericalWave.fileWriter.WriteFile(sphericalWave.gr, "/consistent_results/cons_res_"+
        std::to_string(j-1)+".csv");*/
        sphericalWave.SetJ(j);
        sphericalWave.parameters.fieldSolver(sphericalWave.gr, sphericalWave.parameters.dt);
    }

    TransformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, CtoR);
    sphericalWave.fileWriter.WriteFile(sphericalWave.gr, "/consistent_results/E/cons_res.csv", "writing...");
}

int main(int argc, char** argv) {
    SphericalWave runningWave;
    ParserSphericalWave parser;
    ParametersForSphericalWave params;
    int status = parser.parseArgsForConsistent(argc, argv, params);
    if (status != 0) return 0;
    params.nParSteps = 0;
    params.print();
    runningWave.SetParamsForTest(params);
    TestBody(runningWave);
}