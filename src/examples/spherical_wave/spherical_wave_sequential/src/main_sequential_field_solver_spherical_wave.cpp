#include <string>
#include <iostream>
#include "parser_spherical_wave.h"
#include "spherical_wave.h"
#include "fourier_transform.h"
#include "field_solver.h"
#include "fftw3.h"


void testBody(SphericalWave& sphericalWave) {
    for (int j = 0; j < sphericalWave.parameters.nSeqSteps; j++) {
        sphericalWave.SetJ(j, sphericalWave.gr);
        transformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, RtoC);
        sphericalWave.parameters.fieldSolver(sphericalWave.gr, sphericalWave.parameters.dt);
        transformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, CtoR);
    }

    sphericalWave.parameters.fileWriter.write(sphericalWave.gr, "sequential_result.csv", Double, "writing...");
}

int main(int argc, char** argv) {
    fftw_init_threads();
    SphericalWave sphericalWave;
    ParserSphericalWave parser;
    ParametersForSphericalWave params;
    int status = parser.parseArgsForSequential(argc, argv, params);
    if (status != 0) return 0;
    params.nParSteps = 0;
    params.print();
    sphericalWave.setParamsForTest(params);
    testBody(sphericalWave);
}