#pragma once
#include "mpi_worker.h"
#include "mpi_wrapper_3d.h"
#include "test_parallel.h"
#include "string"
#include "running_wave.h"
#include "field_solver.h"
#include "file_writer.h"

class TestRunningWaveParallel : public TestParallel {
	RunningWave runningWave;
	
public:

    void SetParamsForTest(ParametersForRunningWave p) {
        runningWave.SetParamsForTest(p);
        worker.SetOutput(runningWave.fileWriter, nameFileAfterExchange);
    }

    TestRunningWaveParallel(MPIWrapper3d& _mpiWrapper) : runningWave() {
        runningWave.fileWriter.ChangeDir(runningWave.dir+"parallel_results/");
        SetNameFiles();
        worker.SetOutput(runningWave.fileWriter, nameFileAfterExchange);
        worker.setMPIWrapper3d(_mpiWrapper);
    }

    void DoConsistentPart() {
        FourierTransformation(runningWave.gr, RtoC);
        for (int i = 1; i <= runningWave.parameters.nConsSteps; i++) {
            runningWave.parameters.fieldSolver(runningWave.gr, runningWave.parameters.dt);
        }

        FourierTransformation(runningWave.gr, CtoR);
        //MPIWorker::ShowMessage("writing to file first steps");
        if (MPIWrapper::MPIRank() == 0)
            runningWave.fileWriter.WriteFile(runningWave.gr, nameFileFirstSteps);
    }

    void DoParallelPart() {
        if (worker.Initialize(runningWave.gr, runningWave.parameters.guard,
            runningWave.parameters.mask, runningWave.parameters.maskWidth, worker.getMPIWrapper())==Status::ERROR)
            return;

        MPIWorker::ShowMessage("start par: domain from " + to_string(worker.getMainDomainStart()) + " to " +
            to_string(worker.getMainDomainEnd()) + "; guard is " + to_string(worker.getGuardSize()));

        //MPIWorker::ShowMessage("writing to file first domain");
        runningWave.fileWriter.WriteFile(worker.getGrid(), arrNameFileStartParallelSteps[MPIWrapper::MPIRank()]);

        //MPIWorker::ShowMessage("parallel field solver");
        FieldSolverParallel(worker, runningWave.parameters.fieldSolver, runningWave.parameters.nParSteps, runningWave.parameters.dt, runningWave.parameters.nParSteps, 
            runningWave.fileWriter);

        //MPIWorker::ShowMessage("writing to file parallel result");
        runningWave.fileWriter.WriteFile(worker.getGrid(), arrNameFileFinalParallelSteps[MPIWrapper::MPIRank()]);

        //MPIWorker::ShowMessage("assemble");
        worker.AssembleResultsToZeroProcess(runningWave.gr);
        //MPIWorker::ShowMessage("writing to file assembled result");
        if (MPIWrapper::MPIRank() == 0)
            runningWave.fileWriter.WriteFile(runningWave.gr, nameFileSecondSteps);
    }

    virtual void TestBody() {
        DoConsistentPart();
        DoParallelPart();
    }

};