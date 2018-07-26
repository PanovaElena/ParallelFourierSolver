#pragma once
#include "test_parallel.h"
#include "string"
#include "standing_wave.h"
#include "mpi_worker.h"
#include "field_solver.h"
#include "write_file.h"

class TestStandingWaveParallel : public TestParallel {
public:

    StandingWave standingWave;

    TestStandingWaveParallel() : standingWave() {
        SetNameFiles(standingWave.dir + "parallel_results/");
        worker.SetOutput(standingWave.writeFile, nameFileAfterExchange);
    }

    void DoConsistentPart() {
        for (int i = 0; i <= standingWave.NStartSteps; i++) {
            FieldSolver(standingWave.gr, standingWave.dt);
        }

        FourierTransformation(standingWave.gr, CtoR);
        MPIWorker::ShowMessage("writing to file first steps");
        if (MPIWrapper::MPIRank() == 0)
            standingWave.writeFile(E, y, standingWave.gr, nameFileFirstSteps);
    }

    void DoParallelPart() {
        worker.Initialize(standingWave.gr, standingWave.guard);

        MPIWorker::ShowMessage("writing to file first domain");
        standingWave.writeFile(E, y, worker.getGrid(), arrNameFileStartParallelSteps[MPIWrapper::MPIRank()]);

        MPIWorker::ShowMessage("parallel field solver");
        FieldSolverParallel(worker, standingWave.NNextSteps, standingWave.dt, standingWave.NNextSteps,
            dir, standingWave.writeFile);

        MPIWorker::ShowMessage("writing to file parallel result");
        standingWave.writeFile(E, y, worker.getGrid(), arrNameFileFinalParallelSteps[MPIWrapper::MPIRank()]);

        MPIWorker::ShowMessage("assemble");
        worker.AssembleResultsToZeroProcess(standingWave.gr);
        MPIWorker::ShowMessage("writing to file assembled result");
        if (MPIWrapper::MPIRank() == 0)
            standingWave.writeFile(E, y, standingWave.gr, nameFileSecondSteps);
    }

    virtual void TestBody() {
        MPIWorker::ShowMessage("start: size=" + std::to_string(MPIWrapper::MPISize()) +
            ", n=" + std::to_string(standingWave.nx) + ", guard=" + std::to_string(standingWave.guard));
        DoConsistentPart();
        DoParallelPart();
    }

};