#include <string>
#include <iostream>
#include <omp.h>
#include "parser_running_wave.h"
#include "running_wave.h"
#include "fourier_transform.h"
#include "field_solver.h"
#include "fftw3.h"

void testBody(RunningWave& runningWave) {

    double t1 = omp_get_wtime();

    transformGridIfNecessary(runningWave.params.fieldSolver, runningWave.gr, RtoC);

    for (int j = 0; j < runningWave.params.nSeqSteps; j++) {
        runningWave.params.fieldSolver(runningWave.gr, runningWave.params.dt);
    }

    transformGridIfNecessary(runningWave.params.fieldSolver, runningWave.gr, CtoR);
	
    double t2 = omp_get_wtime();

    std::cout << "Time of sequential version is " << t2 - t1 << std::endl;

    runningWave.params.fileWriter.write(runningWave.gr, "sequential_result.csv", Double, "writing...");
}

int main(int argc, char** argv) {
    fftw_init_threads();
    RunningWave runningWave;
    ParserRunningWave parser;
    ParametersForRunningWave params;
    int status = parser.parseArgsForSequential(argc, argv, params);
    if (status != 0) return 0;
    params.nParSteps = 0;
    params.print();
    runningWave.setParamsForTest(params);
    testBody(runningWave);
}