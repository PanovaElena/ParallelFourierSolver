#pragma once
#include "mpi_worker.h"
#include "test_parallel.h"
#include <string>
#include "spherical_wave.h"
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
        //MPIWorker::ShowMessage("do first steps");
        for (int i = 1; i <= sphericalWave.parameters.nConsSteps; i++) {
            sphericalWave.SetJ(i);
            sphericalWave.parameters.fieldSolver(sphericalWave.gr, sphericalWave.parameters.dt);
        }

        FourierTransformation(sphericalWave.gr, CtoR);
        //MPIWorker::ShowMessage("writing to file first steps");
        if (MPIWrapper::MPIRank() == 0)
            sphericalWave.fileWriter.WriteFile(sphericalWave.gr, nameFileFirstSteps);
    }

    void DoParallelPart() {
        worker.Initialize(sphericalWave.gr, sphericalWave.parameters.guard, sphericalWave.parameters.mask, sphericalWave.parameters.maskWidth);

        //MPIWorker::ShowMessage("writing to file first domain");
        sphericalWave.fileWriter.WriteFile(worker.getGrid(), arrNameFileStartParallelSteps[MPIWrapper::MPIRank()]);

        //MPIWorker::ShowMessage("parallel field solver");
        FieldSolverParallel(worker, sphericalWave.parameters.fieldSolver, sphericalWave.parameters.nParSteps, sphericalWave.parameters.dt, sphericalWave.parameters.nParSteps, 
            sphericalWave.fileWriter);

        //MPIWorker::ShowMessage("writing to file parallel result");
        sphericalWave.fileWriter.WriteFile(worker.getGrid(), arrNameFileFinalParallelSteps[MPIWrapper::MPIRank()]);

        //MPIWorker::ShowMessage("assemble");
        worker.AssembleResultsToZeroProcess(sphericalWave.gr);
        //MPIWorker::ShowMessage("writing to file assembled result");
        if (MPIWrapper::MPIRank() == 0)
            sphericalWave.fileWriter.WriteFile(sphericalWave.gr, nameFileSecondSteps);    
    }

    virtual void TestBody() {
        MPIWorker::ShowMessage("start: size=" + std::to_string(MPIWrapper::MPISize()) +
            ", n=" + std::to_string(sphericalWave.parameters.nx) + ", guard=" + std::to_string(sphericalWave.parameters.guard));
        DoConsistentPart();
        DoParallelPart();
    }

};
