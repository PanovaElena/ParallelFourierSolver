#pragma once
#include "mpi_worker.h"
#include "test_parallel.h"
#include <string>
#include "spherical_wave.h"
#include "field_solver.h"
#include "file_writer.h"

class TestSphericalWaveParallel : public TestParallel {

    SphericalWave sphericalWave;
    MPIWorker& worker;

public:
    void SetParamsForTest(ParametersForSphericalWave p) {
        sphericalWave.SetParamsForTest(p);
    }

    TestSphericalWaveParallel(MPIWorker& _mpiWorker) : sphericalWave(), worker(_mpiWorker) {
        SetNameFiles();
    }

    void DoSequentialPart() {
        //MPIWorker::ShowMessage("do first steps");
        TransformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, RtoC);
        for (int i = 1; i <= sphericalWave.parameters.nSeqSteps; i++) {
            TransformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, CtoR);
            sphericalWave.SetJ(i);
            TransformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, RtoC);
            sphericalWave.parameters.fieldSolver(sphericalWave.gr, sphericalWave.parameters.dt);
        }
        TransformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, CtoR);

        //MPIWorker::ShowMessage("writing to file first steps");
        if (MPIWrapper::MPIRank() == 0)
            sphericalWave.parameters.fileWriter.WriteFile(sphericalWave.gr, nameFileFirstSteps);
    }

    void DoParallelPart() {
        //MPIWorker::ShowMessage("start init worker");
        vec3<int> g(worker.getMPIWrapper().MPISize().x == 1 ? 0 : sphericalWave.parameters.guard.x,
            worker.getMPIWrapper().MPISize().y == 1 ? 0 : sphericalWave.parameters.guard.y,
            worker.getMPIWrapper().MPISize().z == 1 ? 0 : sphericalWave.parameters.guard.z);
        if (worker.Initialize(sphericalWave.gr, g,
            sphericalWave.parameters.mask, worker.getMPIWrapper()) == Status::ERROR)
            return;

        MPIWorker::ShowMessage("start par: domain from " + to_string(worker.getMainDomainStart()) + " to " +
            to_string(worker.getMainDomainEnd()) + "; guard is " + to_string(worker.getGuardSize()));

        //MPIWorker::ShowMessage("writing to file first domain");
        sphericalWave.parameters.fileWriter.WriteFile(worker.getGrid(), nameFileAfterDivision);

        //MPIWorker::ShowMessage("parallel field solver");
        FieldSolverParallel(worker, sphericalWave.parameters.fieldSolver, sphericalWave.parameters.nParSteps, sphericalWave.parameters.nDomainSteps,
            sphericalWave.parameters.dt, sphericalWave.parameters.fileWriter);

        //MPIWorker::ShowMessage("writing to file parallel result");
        sphericalWave.parameters.fileWriter.WriteFile(worker.getGrid(), nameFileAfterExchange);

        //MPIWorker::ShowMessage("assemble");
        worker.AssembleResultsToZeroProcess(sphericalWave.gr);

        if (sphericalWave.parameters.filter.state == Filter::on && MPIWrapper::MPIRank() == 0) {
            TransformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, RtoC);
            sphericalWave.parameters.filter(sphericalWave.gr);
            TransformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, CtoR);
        }

        //MPIWorker::ShowMessage("writing to file assembled result");
        if (MPIWrapper::MPIRank() == 0)
            sphericalWave.parameters.fileWriter.WriteFile(sphericalWave.gr, nameFileSecondSteps);
    }

    virtual void TestBody() {
        DoSequentialPart();
        MPIWrapper::MPIBarrier();
        DoParallelPart();
    }

};
