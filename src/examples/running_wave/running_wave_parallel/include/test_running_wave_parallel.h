#pragma once
#include <omp.h>
#include "mpi_worker.h"
#include "mpi_wrapper3d.h"
#include "test_parallel.h"
#include "string"
#include "running_wave.h"
#include "field_solver.h"
#include "file_writer.h"

class TestRunningWaveParallel : public TestParallel {
    RunningWave runningWave;
    MPIWorker& worker;

public:
    void setParamsForTest(ParametersForRunningWave p) {
        runningWave.setParamsForTest(p);
    }


    TestRunningWaveParallel(MPIWorker& _mpiWorker) : runningWave(), worker(_mpiWorker) {
        setNameFiles();
    }

    void doSequentialPart() {
        fourierTransformation(runningWave.gr, RtoC);
        for (int i = 1; i <= runningWave.parameters.nSeqSteps; i++) {
            runningWave.parameters.fieldSolver(runningWave.gr, runningWave.parameters.dt);
        }
        fourierTransformation(runningWave.gr, CtoR);

        //MPIWorker::showMessage("writing to file first steps");
        if (MPIWrapper::MPIRank() == 0)
            runningWave.parameters.fileWriter.write(runningWave.gr, nameFileFirstSteps);
    }

    Status doParallelPart() {
        vec3<int> g(worker.getMPIWrapper().MPISize().x == 1 ? 0 : runningWave.parameters.guard.x,
            worker.getMPIWrapper().MPISize().y == 1 ? 0 : runningWave.parameters.guard.y,
            worker.getMPIWrapper().MPISize().z == 1 ? 0 : runningWave.parameters.guard.z);
        if (worker.initialize(runningWave.gr, g,
            runningWave.parameters.mask, worker.getMPIWrapper()) == Status::ERROR)
            return Status::ERROR;

        //MPIWorker::showMessage("start par: domain from " + to_string(worker.getMainDomainStart()) + " to " +
           // to_string(worker.getMainDomainEnd()) + "; guard is " + to_string(worker.getGuardSize()));

        //MPIWorker::showMessage("writing to file first domain");
        runningWave.parameters.fileWriter.write(worker.getGrid(), nameFileAfterDivision);

        double t1 = omp_get_wtime();

        //MPIWorker::showMessage("parallel field solver");
        spectralSolverParallel(worker, runningWave.parameters.fieldSolver, runningWave.parameters.nParSteps, runningWave.parameters.nDomainSteps,
            runningWave.parameters.dt, runningWave.parameters.filter, runningWave.parameters.fileWriter);

        double t2 = omp_get_wtime();

        if (MPIWrapper::MPIRank() == 0)
            std::cout << "Time of parallel version is " << t2 - t1 << std::endl;

        //MPIWorker::showMessage("writing to file parallel result");
        runningWave.parameters.fileWriter.write(worker.getGrid(), nameFileAfterExchange);

        //MPIWorker::showMessage("assemble");
        worker.assembleResultsToZeroProcess(runningWave.gr);

        //MPIWorker::showMessage("writing to file assembled result");
        if (MPIWrapper::MPIRank() == 0)
            runningWave.parameters.fileWriter.write(runningWave.gr, nameFileSecondSteps);

        return Status::OK;
    }

    virtual Status testBody() {
        if (runningWave.parameters.nSeqSteps != 0)
            doSequentialPart();
        MPIWrapper::MPIBarrier();
        if (runningWave.parameters.nParSteps != 0)
            return doParallelPart();
        return Status::OK;
    }

};