#include <string>
#include <iostream>
#include "parser_running_wave.h"
#include "running_wave.h"
#include "fourier_transformation.h"
#include "field_solver.h"

void TestBody(RunningWave& runningWave) {

    TransformGridIfNecessary(runningWave.parameters.fieldSolver, runningWave.gr, RtoC);
    for (int j = 1; j <= runningWave.parameters.nSeqSteps; j++) {
        runningWave.parameters.fieldSolver(runningWave.gr, runningWave.parameters.dt);
    }

    TransformGridIfNecessary(runningWave.parameters.fieldSolver, runningWave.gr, CtoR);
    runningWave.parameters.fileWriter.WriteFile(runningWave.gr, "sequential_result.csv", Double, "writing...");
}

int main(int argc, char** argv) {
    RunningWave runningWave;
    ParserRunningWave parser;
    ParametersForRunningWave params;
    int status = parser.parseArgsForSequential(argc, argv, params);
    if (status != 0) return 0;
    params.nParSteps = 0;
    params.print();
    runningWave.SetParamsForTest(params);
    TestBody(runningWave);
}