#pragma once
#include "test_parallel.h"
#include "string"
#include "spherical_wave.h"
#include "mpi_worker.h"
#include "field_solver.h"
#include "write_file.h"

class TestSphericalWaveParallel : public TestParallel {
public:

    SphericalWave sphericalWave;

    TestSphericalWaveParallel(): sphericalWave(){
        SetNameFiles(sphericalWave.dir + "parallel_results/");
        worker.SetOutput(sphericalWave.writeFile, nameFileAfterExchange);
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
            sphericalWave.writeFile(E, z, sphericalWave.gr, nameFileFirstSteps);
    }

    void DoParallelPart() {
        worker.Initialize(sphericalWave.gr, sphericalWave.guard);

        MPIWorker::ShowMessage("writing to file first domain");
        sphericalWave.writeFile(E, z, worker.getGrid(), arrNameFileStartParallelSteps[MPIWrapper::MPIRank()]);

        MPIWorker::ShowMessage("parallel field solver");
        FieldSolverParallel(worker, sphericalWave.NNextSteps, sphericalWave.dt, sphericalWave.NNextSteps, 
            dir, sphericalWave.writeFile);

        MPIWorker::ShowMessage("writing to file parallel result");
        sphericalWave.writeFile(E, z, worker.getGrid(), arrNameFileFinalParallelSteps[MPIWrapper::MPIRank()]);

        MPIWorker::ShowMessage("assemble");
        worker.AssembleResultsToZeroProcess(sphericalWave.gr);
        MPIWorker::ShowMessage("writing to file assembled result");
        if (MPIWrapper::MPIRank() == 0)
            sphericalWave.writeFile(E, z, sphericalWave.gr, nameFileSecondSteps);    
    }

    virtual void TestBody() {
        MPIWorker::ShowMessage("start: size=" + std::to_string(MPIWrapper::MPISize()) +
            ", n=" + std::to_string(sphericalWave.nx) + ", guard=" + std::to_string(sphericalWave.guard));
        DoConsistentPart();
        DoParallelPart();
    }

};
