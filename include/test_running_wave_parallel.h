#pragma once
#include "test_parallel.h"
#include "string"
#include "running_wave.h"
#include "mpi_worker.h"
#include "field_solver.h"
#include "write_file.h"

class TestRunningWaveParallel : public TestParallel {
public:

    RunningWave runningWave;

    TestRunningWaveParallel() : runningWave(){
        SetNameFiles(runningWave.dir+"parallel_results/");
        worker.SetOutput(runningWave.writeFile, nameFileAfterExchange);
    }

    void DoConsistentPart() {
        for (int i = 0; i <= runningWave.NStartSteps; i++) {
            FieldSolver(runningWave.gr, runningWave.dt);
        }

        FourierTransformation(runningWave.gr, CtoR);
        MPIWorker::ShowMessage("writing to file first steps");
        if (MPIWrapper::MPIRank() == 0)
            runningWave.writeFile(E, y, runningWave.gr, nameFileFirstSteps);
    }

    void DoParallelPart() {
        worker.Initialize(runningWave.gr, runningWave.guard);

        MPIWorker::ShowMessage("writing to file first domain");
        runningWave.writeFile(E, y, worker.getGrid(), arrNameFileStartParallelSteps[MPIWrapper::MPIRank()]);

        MPIWorker::ShowMessage("parallel field solver");
        FieldSolverParallel(worker, runningWave.NNextSteps, runningWave.dt, runningWave.NNextSteps, 
            dir, runningWave.writeFile);

        MPIWorker::ShowMessage("writing to file parallel result");
        runningWave.writeFile(E, y, worker.getGrid(), arrNameFileFinalParallelSteps[MPIWrapper::MPIRank()]);

        MPIWorker::ShowMessage("assemble");
        worker.AssembleResultsToZeroProcess(runningWave.gr);
        MPIWorker::ShowMessage("writing to file assembled result");
        if (MPIWrapper::MPIRank() == 0)
            runningWave.writeFile(E, y, runningWave.gr, nameFileSecondSteps);
    }

    virtual void TestBody() {
        MPIWorker::ShowMessage("start: size=" + std::to_string(MPIWrapper::MPISize()) +
            ", n=" + std::to_string(runningWave.nx) + ", guard=" + std::to_string(runningWave.guard));
        DoConsistentPart();
        DoParallelPart();
    }

};