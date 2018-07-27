#pragma once
#include "test_parallel.h"
#include "string"
#include "spherical_wave.h"
#include "mpi_worker.h"
#include "field_solver.h"
#include "file_writer.h"

class TestSphericalWaveParallel : public TestParallel {
public:

    SphericalWave sphericalWave;

    TestSphericalWaveParallel() : sphericalWave() {
        sphericalWave.fileWriter.ChangeDir(sphericalWave.dir + "parallel_results/");
        SetNameFiles();
        worker.SetOutput(sphericalWave.fileWriter, nameFileAfterExchange);
    }

    void DoConsistentPart() {
        MPIWorker::ShowMessage("do first steps");
        for (int i = 1; i <= sphericalWave.NStartSteps; i++) {
            sphericalWave.SetJ(i);
            FieldSolver(sphericalWave.gr, sphericalWave.dt);
        }

        FourierTransformation(sphericalWave.gr, CtoR);
        MPIWorker::ShowMessage("writing to file first steps");
        if (MPIWrapper::MPIRank() == 0)
            sphericalWave.fileWriter.WriteFile(sphericalWave.gr, nameFileFirstSteps);
    }

    void DoParallelPart() {
        worker.Initialize(sphericalWave.gr, sphericalWave.guard, sphericalWave.mask);

        MPIWorker::ShowMessage("writing to file first domain");
        sphericalWave.fileWriter.WriteFile(worker.getGrid(), arrNameFileStartParallelSteps[MPIWrapper::MPIRank()]);

        MPIWorker::ShowMessage("parallel field solver");
        FieldSolverParallel(worker, sphericalWave.NNextSteps, sphericalWave.dt, sphericalWave.NNextSteps, 
            sphericalWave.fileWriter);

        MPIWorker::ShowMessage("writing to file parallel result");
        sphericalWave.fileWriter.WriteFile(worker.getGrid(), arrNameFileFinalParallelSteps[MPIWrapper::MPIRank()]);

        MPIWorker::ShowMessage("assemble");
        worker.AssembleResultsToZeroProcess(sphericalWave.gr);
        MPIWorker::ShowMessage("writing to file assembled result");
        if (MPIWrapper::MPIRank() == 0)
            sphericalWave.fileWriter.WriteFile(sphericalWave.gr, nameFileSecondSteps);    
    }

    virtual void TestBody() {
        MPIWorker::ShowMessage("start: size=" + std::to_string(MPIWrapper::MPISize()) +
            ", n=" + std::to_string(sphericalWave.nx) + ", guard=" + std::to_string(sphericalWave.guard));
        DoConsistentPart();
        DoParallelPart();
    }

};
