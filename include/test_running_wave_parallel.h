#pragma once
#include "test_parallel.h"
#include "string"
#include "running_wave.h"
#include "mpi_worker.h"
#include "field_solver.h"
#include "file_writer.h"

class TestRunningWaveParallel : public TestParallel {
public:

    RunningWave runningWave;

    TestRunningWaveParallel() : runningWave() {
        runningWave.fileWriter.ChangeDir(runningWave.dir+"parallel_results/");
        SetNameFiles();
        worker.SetOutput(runningWave.fileWriter, nameFileAfterExchange);
    }

    void DoConsistentPart() {
        for (int i = 1; i <= runningWave.nStartSteps; i++) {
            runningWave.fieldSolver(runningWave.gr, runningWave.dt);
        }

        FourierTransformation(runningWave.gr, CtoR);
        //MPIWorker::ShowMessage("writing to file first steps");
        if (MPIWrapper::MPIRank() == 0)
            runningWave.fileWriter.WriteFile(runningWave.gr, nameFileFirstSteps);
    }

    void DoParallelPart() {
        worker.Initialize(runningWave.gr, runningWave.guard, runningWave.mask, runningWave.maskWidth);

        //MPIWorker::ShowMessage("writing to file first domain");
        runningWave.fileWriter.WriteFile(worker.getGrid(), arrNameFileStartParallelSteps[MPIWrapper::MPIRank()]);

        //MPIWorker::ShowMessage("parallel field solver");
        FieldSolverParallel(worker, runningWave.fieldSolver, runningWave.nNextSteps, runningWave.dt, runningWave.nNextSteps, 
            runningWave.fileWriter);

        //MPIWorker::ShowMessage("writing to file parallel result");
        runningWave.fileWriter.WriteFile(worker.getGrid(), arrNameFileFinalParallelSteps[MPIWrapper::MPIRank()]);

        //MPIWorker::ShowMessage("assemble");
        worker.AssembleResultsToZeroProcess(runningWave.gr);
        //MPIWorker::ShowMessage("writing to file assembled result");
        if (MPIWrapper::MPIRank() == 0)
            runningWave.fileWriter.WriteFile(runningWave.gr, nameFileSecondSteps);
    }

    virtual void TestBody() {
        MPIWorker::ShowMessage("start: size=" + std::to_string(MPIWrapper::MPISize()) +
            ", n=" + std::to_string(runningWave.nx) + ", guard=" + std::to_string(runningWave.guard)
            + ", lambda = " + std::to_string(runningWave.lambda) +
            ", num of par steps=" + std::to_string(runningWave.nNextSteps));
        DoConsistentPart();
        DoParallelPart();
    }

};