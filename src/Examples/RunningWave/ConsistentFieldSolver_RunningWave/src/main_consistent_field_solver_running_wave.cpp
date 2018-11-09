#include <string>
#include <iostream>
#include "parser_running_wave.h"
#include "running_wave.h"
#include "fourier_transformation.h"
#include "field_solver.h"

void TestBody(RunningWave& runningWave) {

    TransformGridIfNecessary(runningWave.parameters.fieldSolver, runningWave.gr, RtoC);
    for (int j = 1; j <= runningWave.parameters.nConsSteps; j++) {
        /*FourierTransformation(runningWave.gr, CtoR);
        runningWave.parameters.fileWriter.WriteFile(runningWave.gr, "/consistent_results/cons_res_"+
            std::to_string(j-1)+".csv");*/
        runningWave.parameters.fieldSolver(runningWave.gr, runningWave.parameters.dt);
    }

    TransformGridIfNecessary(runningWave.parameters.fieldSolver, runningWave.gr, CtoR);
    runningWave.fileWriter.WriteFile(runningWave.gr, "/consistent_results/E/cons_res.csv", "writing...");
}

int main(int argc, char** argv) {
    RunningWave runningWave;
    ParserRunningWave parser;
    ParametersForRunningWave params;
    int status = parser.parseArgsForConsistent(argc, argv, params);
    if (status != 0) return 0;
    params.nParSteps = 0;
    params.print();
    runningWave.SetParamsForTest(params);
    TestBody(runningWave);
}