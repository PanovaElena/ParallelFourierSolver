#pragma once
#include <string>
#include "mpi_worker.h"
#include "test_parallel.h"
#include "spherical_wave.h"
#include "field_solver.h"
#include "file_writer.h"

class TestSphericalWaveParallel : public TestParallel {

    SphericalWave sphericalWave;
    MPIWorker& worker;

public:
    void setParamsForTest(ParametersForSphericalWave p) {
        sphericalWave.setParamsForTest(p);
    }

    TestSphericalWaveParallel(MPIWorker& _mpiWorker) : sphericalWave(), worker(_mpiWorker) {
        setNameFiles();
    }

    void doSequentialPart() {
        //MPIWrapper::showMessage("do first steps");
        transformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, RtoC);
        for (int i = 0; i < sphericalWave.parameters.nSeqSteps; i++) {
            transformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, CtoR);
            sphericalWave.SetJ(i, sphericalWave.gr);
            transformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, RtoC);
            sphericalWave.parameters.fieldSolver(sphericalWave.gr, sphericalWave.parameters.dt);
        }
        transformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, CtoR);

        //MPIWrapper::showMessage("writing to file first steps");
        if (MPIWrapper::MPIRank() == 0)
            sphericalWave.parameters.fileWriter.write(sphericalWave.gr, nameFileFirstSteps);
    }

    void computeParallel(MPIWorker& worker) {
        double startTimeOfSource = sphericalWave.parameters.source.startTime;
        double endTimeOfSource = sphericalWave.parameters.source.getEndTime();
        double startTimeOfSeq = 0;
        double endTimeOfSeq = (sphericalWave.parameters.nSeqSteps - 1) * sphericalWave.parameters.dt;
        double startTimeOfPar = sphericalWave.parameters.nSeqSteps * sphericalWave.parameters.dt;
        double endTimeOfPar = sphericalWave.parameters.getNSteps() * sphericalWave.parameters.dt;

        int n1 = 0, n2 = 0;
        if (endTimeOfSource > endTimeOfSeq) {
            n1 = sphericalWave.parameters.nParSteps;
            n2 = 1;
        }
        else {
            n1 = 1;
            n2 = sphericalWave.parameters.nParSteps;;
        }

        for (int iter = 0; iter < n1; iter++)
            spectralSolverParallel(worker, sphericalWave.parameters.fieldSolver, n2,
                sphericalWave.parameters.nDomainSteps, sphericalWave.parameters.dt,
                sphericalWave.parameters.fileWriter);
    }

    Status doParallelPart() {
        //MPIWrapper::showMessage("start init worker");
        vec3<int> g(worker.getMPIWrapper().MPISize().x == 1 ? 0 : sphericalWave.parameters.guard.x,
            worker.getMPIWrapper().MPISize().y == 1 ? 0 : sphericalWave.parameters.guard.y,
            worker.getMPIWrapper().MPISize().z == 1 ? 0 : sphericalWave.parameters.guard.z);
        if (worker.initialize(sphericalWave.gr, g,
            sphericalWave.parameters.mask, worker.getMPIWrapper()) == Status::ERROR)
            return Status::ERROR;

        //MPIWrapper::showMessage("start par: domain from " + to_string(worker.getMainDomainStart()) + " to " +
            //to_string(worker.getMainDomainEnd()) + "; guard is " + to_string(worker.getGuardSize()));

        //MPIWrapper::showMessage("writing to file first domain");
        sphericalWave.parameters.fileWriter.write(worker.getGrid(), nameFileAfterDivision);

        //MPIWrapper::showMessage("parallel field solver");
        computeParallel(worker);

        //MPIWrapper::showMessage("writing to file parallel result");
        sphericalWave.parameters.fileWriter.write(worker.getGrid(), nameFileAfterExchange);

        //MPIWrapper::showMessage("assemble");
        worker.assembleResultsToZeroProcess(sphericalWave.gr);

        if (sphericalWave.parameters.filter.state == Filter::on && MPIWrapper::MPIRank() == 0) {
            transformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, RtoC);
            sphericalWave.parameters.filter(sphericalWave.gr);
            transformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, CtoR);
        }

        //MPIWrapper::showMessage("writing to file assembled result");
        if (MPIWrapper::MPIRank() == 0)
            sphericalWave.parameters.fileWriter.write(sphericalWave.gr, nameFileSecondSteps);

        return Status::OK;
    }

    virtual Status testBody() {
        if (sphericalWave.parameters.nSeqSteps != 0)
            doSequentialPart();
        MPIWrapper::MPIBarrier();
        if (sphericalWave.parameters.nParSteps != 0)
            return doParallelPart();
        return Status::OK;
    }

};
