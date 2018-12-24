#pragma once
#include "mpi_worker.h"
#include "test_parallel.h"
#include <string>
#include "spherical_wave.h"
#include "field_solver.h"
#include "file_writer.h"

class TestSphericalWaveParallel : public TestParallel {
	
SphericalWave sphericalWave;

public:
    void SetParamsForTest(ParametersForSphericalWave p) {
        sphericalWave.SetParamsForTest(p);
        worker.SetOutput(sphericalWave.fileWriter, nameFileAfterExchange);
    }

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
        vec3<int> g(worker.getMPIWrapper().MPISize().x == 1 ? 0 : sphericalWave.parameters.guard.x,
            worker.getMPIWrapper().MPISize().y == 1 ? 0 : sphericalWave.parameters.guard.y,
            worker.getMPIWrapper().MPISize().z == 1 ? 0 : sphericalWave.parameters.guard.z);
        if (worker.Initialize(sphericalWave.gr, g,
            sphericalWave.parameters.mask, worker.getMPIWrapper()) == Status::ERROR)
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

        if (sphericalWave.parameters.filter.state == Filter::on && MPIWrapper::MPIRank() == 0) {
            //TransformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, RtoC);
            //sphericalWave.parameters.filter(sphericalWave.gr);
            //TransformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, CtoR);
        }

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
