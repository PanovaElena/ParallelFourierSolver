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

    TestSphericalWaveParallel(MPIWrapper3d& _mpiWrapper) : sphericalWave() {
        sphericalWave.fileWriter.ChangeDir(sphericalWave.dir + "parallel_results/");
        SetNameFiles();
        worker.SetOutput(sphericalWave.fileWriter, nameFileAfterExchange);
        worker.setMPIWrapper3d(_mpiWrapper);
    }

    void DoConsistentPart() {
        //MPIWorker::ShowMessage("do first steps");
        FourierTransformation(sphericalWave.gr, RtoC);
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
        //MPIWorker::ShowMessage("start init worker");
        if (worker.Initialize(sphericalWave.gr, sphericalWave.parameters.guard, 
            sphericalWave.parameters.mask, sphericalWave.parameters.maskWidth, worker.getMPIWrapper())==1)
            return;

        MPIWorker::ShowMessage("start par: domain from " +to_string(worker.getMainDomainStart())+" to "+
            to_string(worker.getMainDomainEnd()) + "; guard is " + to_string(worker.getGuardSize()));

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
        DoConsistentPart();
        MPIWrapper::MPIBarrier();
        DoParallelPart();
    }

};
