#pragma once
#include "mpi_worker.h"
#include "mpi_wrapper_3d.h"
#include <omp.h>
#include "test_parallel.h"
#include "string"
#include "running_wave.h"
#include "field_solver.h"
#include "file_writer.h"

class TestRunningWaveParallel : public TestParallel {

    RunningWave runningWave;
    MPIWorker& worker;

public:

    void SetParamsForTest(ParametersForRunningWave p) {
        runningWave.SetParamsForTest(p);
    }


    TestRunningWaveParallel(MPIWorker& _mpiWorker) : runningWave(), worker(_mpiWorker) {
        SetNameFiles();
    }

    void DoSequentialPart() {
        FourierTransformation(runningWave.gr, RtoC);
        for (int i = 1; i <= runningWave.parameters.nSeqSteps; i++) {
            runningWave.parameters.fieldSolver(runningWave.gr, runningWave.parameters.dt);
        }
        FourierTransformation(runningWave.gr, CtoR);

        //MPIWorker::ShowMessage("writing to file first steps");
        if (MPIWrapper::MPIRank() == 0)
            runningWave.parameters.fileWriter.WriteFile(runningWave.gr, nameFileFirstSteps);
    }

    void DoParallelPart() {
        vec3<int> g(worker.getMPIWrapper().MPISize().x == 1 ? 0 : runningWave.parameters.guard.x,
            worker.getMPIWrapper().MPISize().y == 1 ? 0 : runningWave.parameters.guard.y,
            worker.getMPIWrapper().MPISize().z == 1 ? 0 : runningWave.parameters.guard.z);
        if (worker.Initialize(runningWave.gr, g,
            runningWave.parameters.mask, worker.getMPIWrapper()) == Status::ERROR)
            return;

        MPIWorker::ShowMessage("start par: domain from " + to_string(worker.getMainDomainStart()) + " to " +
            to_string(worker.getMainDomainEnd()) + "; guard is " + to_string(worker.getGuardSize()));

        //MPIWorker::ShowMessage("writing to file first domain");
        runningWave.parameters.fileWriter.WriteFile(worker.getGrid(), nameFileAfterDivision);

		double t1 = omp_get_wtime();

        //MPIWorker::ShowMessage("parallel field solver");
        FieldSolverParallel(worker, runningWave.parameters.fieldSolver, runningWave.parameters.nParSteps, runningWave.parameters.nDomainSteps,
            runningWave.parameters.dt, runningWave.parameters.fileWriter);

		double t2 = omp_get_wtime();
        if (MPIWrapper::MPIRank() == 0)
            std::cout << "Time of parallel version is " << t2 - t1 << std::endl;

        //MPIWorker::ShowMessage("writing to file parallel result");
        runningWave.parameters.fileWriter.WriteFile(worker.getGrid(), nameFileAfterExchange);

        //MPIWorker::ShowMessage("assemble");
        worker.AssembleResultsToZeroProcess(runningWave.gr);

        if (runningWave.parameters.filter.state == Filter::on && MPIWrapper::MPIRank() == 0) {
            TransformGridIfNecessary(runningWave.parameters.fieldSolver, runningWave.gr, RtoC);
            runningWave.parameters.filter(runningWave.gr);
            runningWave.parameters.fileWriter.WriteFile(runningWave.gr, "spectrum.csv", Complex);
            TransformGridIfNecessary(runningWave.parameters.fieldSolver, runningWave.gr, CtoR);
        }

        //MPIWorker::ShowMessage("writing to file assembled result");
        if (MPIWrapper::MPIRank() == 0)
            runningWave.parameters.fileWriter.WriteFile(runningWave.gr, nameFileSecondSteps);
    }

    virtual void TestBody() {
        DoSequentialPart();
        DoParallelPart();
    }

};