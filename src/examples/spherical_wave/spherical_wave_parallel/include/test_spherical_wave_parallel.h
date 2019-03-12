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
        //MPIWorker::showMessage("do first steps");
        transformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, RtoC);
        for (int i = 0; i < sphericalWave.parameters.nSeqSteps; i++) {
            transformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, CtoR);
            sphericalWave.SetJ(i, sphericalWave.gr);
            transformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, RtoC);
            sphericalWave.parameters.fieldSolver(sphericalWave.gr, sphericalWave.parameters.dt);
        }
        transformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, CtoR);

        //MPIWorker::showMessage("writing to file first steps");
        if (MPIWrapper::MPIRank() == 0)
            sphericalWave.parameters.fileWriter.write(sphericalWave.gr, nameFileFirstSteps);
    }

    void computeParallel(MPIWorker& worker) {
        double startTimeOfSource = sphericalWave.parameters.source.getStartTime();
        double endTimeOfSource = sphericalWave.parameters.source.getEndTime();
        double startTimeOfSeq = 0;
        double endTimeOfSeq = (sphericalWave.parameters.nSeqSteps - 1) * sphericalWave.parameters.dt;
        double startTimeOfPar = sphericalWave.parameters.nSeqSteps * sphericalWave.parameters.dt;
        double endTimeOfPar = sphericalWave.parameters.getNSteps() * sphericalWave.parameters.dt;

        int nIter1 = 0, nIter2 = 0, nIter3 = 0;
        if (startTimeOfSource >= startTimeOfPar)
            nIter1 = (int)((startTimeOfSource - startTimeOfPar) / sphericalWave.parameters.dt + (1 - 1.0e-10));
        if (endTimeOfSource >= startTimeOfPar)
            nIter2 = (int)((endTimeOfSource - startTimeOfPar) / sphericalWave.parameters.dt) - nIter1;
        nIter3 = sphericalWave.parameters.nParSteps - nIter2 - nIter1;
        const int N_ITER_SAVE = (int)(0.1*nIter3);

        // part 1
        spectralSolverParallel(worker, sphericalWave.parameters.fieldSolver, nIter1,
            sphericalWave.parameters.nDomainSteps, sphericalWave.parameters.dt, sphericalWave.parameters.fileWriter);
        // part 2
        for (int i = nIter1 + sphericalWave.parameters.nSeqSteps;
            i < nIter1 + nIter2 + N_ITER_SAVE + sphericalWave.parameters.nSeqSteps; i++) {
            sphericalWave.SetJ(i, worker.getGrid());
            spectralSolverParallel(worker, sphericalWave.parameters.fieldSolver, 1,
                sphericalWave.parameters.nDomainSteps, sphericalWave.parameters.dt, sphericalWave.parameters.fileWriter);
        }
        // part 3
        spectralSolverParallel(worker, sphericalWave.parameters.fieldSolver, nIter3 - N_ITER_SAVE,
            sphericalWave.parameters.nDomainSteps, sphericalWave.parameters.dt, sphericalWave.parameters.fileWriter);
    }

    void doParallelPart() {
        //MPIWorker::showMessage("start init worker");
        vec3<int> g(worker.getMPIWrapper().MPISize().x == 1 ? 0 : sphericalWave.parameters.guard.x,
            worker.getMPIWrapper().MPISize().y == 1 ? 0 : sphericalWave.parameters.guard.y,
            worker.getMPIWrapper().MPISize().z == 1 ? 0 : sphericalWave.parameters.guard.z);
        if (worker.initialize(sphericalWave.gr, g,
            sphericalWave.parameters.mask, worker.getMPIWrapper()) == Status::ERROR)
            return;

        MPIWorker::showMessage("start par: domain from " + to_string(worker.getMainDomainStart()) + " to " +
            to_string(worker.getMainDomainEnd()) + "; guard is " + to_string(worker.getGuardSize()));

        //MPIWorker::showMessage("writing to file first domain");
        sphericalWave.parameters.fileWriter.write(worker.getGrid(), nameFileAfterDivision);

        //MPIWorker::showMessage("parallel field solver");
        computeParallel(worker);

        //MPIWorker::showMessage("writing to file parallel result");
        sphericalWave.parameters.fileWriter.write(worker.getGrid(), nameFileAfterExchange);

        //MPIWorker::showMessage("assemble");
        worker.assembleResultsToZeroProcess(sphericalWave.gr);

        if (sphericalWave.parameters.filter.state == Filter::on && MPIWrapper::MPIRank() == 0) {
            transformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, RtoC);
            sphericalWave.parameters.filter(sphericalWave.gr);
            transformGridIfNecessary(sphericalWave.parameters.fieldSolver, sphericalWave.gr, CtoR);
        }

        //MPIWorker::showMessage("writing to file assembled result");
        if (MPIWrapper::MPIRank() == 0)
            sphericalWave.parameters.fileWriter.write(sphericalWave.gr, nameFileSecondSteps);
    }

    virtual void testBody() {
        if (sphericalWave.parameters.nSeqSteps != 0)
            doSequentialPart();
        MPIWrapper::MPIBarrier();
        if (sphericalWave.parameters.nParSteps != 0)
            doParallelPart();
    }

};
