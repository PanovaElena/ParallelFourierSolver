#pragma once
#pragma once
#include "test_parallel.h"
#include "string"
#include "mpi_worker.h"
#include "field_solver.h"
#include "file_writer.h"
#include "running_wave_with_high_frequency.h"

class TestRunningWaveWithHighFrequencyParallel : public TestParallel {
public:

    RunningWaveWithHighFrequency runningWaveWithHighFrequency;

    TestRunningWaveWithHighFrequencyParallel() : runningWaveWithHighFrequency() {
        runningWaveWithHighFrequency.fileWriter.ChangeDir(runningWaveWithHighFrequency.dir + "parallel_results/");
        SetNameFiles();
        worker.SetOutput(runningWaveWithHighFrequency.fileWriter, nameFileAfterExchange);
    }

    void DoConsistentPart() {
        for (int i = 0; i <= runningWaveWithHighFrequency.NStartSteps; i++) {
            FieldSolver(runningWaveWithHighFrequency.gr, runningWaveWithHighFrequency.dt);
        }

        FourierTransformation(runningWaveWithHighFrequency.gr, CtoR);
        MPIWorker::ShowMessage("writing to file first steps");
        if (MPIWrapper::MPIRank() == 0)
            runningWaveWithHighFrequency.fileWriter.WriteFile(runningWaveWithHighFrequency.gr, nameFileFirstSteps);
    }

    void DoParallelPart() {
        worker.Initialize(runningWaveWithHighFrequency.gr, runningWaveWithHighFrequency.guard,
            runningWaveWithHighFrequency.mask);

        MPIWorker::ShowMessage("writing to file first domain");
        runningWaveWithHighFrequency.fileWriter.WriteFile(worker.getGrid(), arrNameFileStartParallelSteps[MPIWrapper::MPIRank()]);

        MPIWorker::ShowMessage("parallel field solver");
        FieldSolverParallel(worker, runningWaveWithHighFrequency.NNextSteps, runningWaveWithHighFrequency.dt, runningWaveWithHighFrequency.NNextSteps,
            runningWaveWithHighFrequency.fileWriter);

        MPIWorker::ShowMessage("writing to file parallel result");
        runningWaveWithHighFrequency.fileWriter.WriteFile(worker.getGrid(), arrNameFileFinalParallelSteps[MPIWrapper::MPIRank()]);

        MPIWorker::ShowMessage("assemble");
        worker.AssembleResultsToZeroProcess(runningWaveWithHighFrequency.gr);
        MPIWorker::ShowMessage("writing to file assembled result");
        if (MPIWrapper::MPIRank() == 0)
            runningWaveWithHighFrequency.fileWriter.WriteFile(runningWaveWithHighFrequency.gr, nameFileSecondSteps);
    }

    virtual void TestBody() {
        MPIWorker::ShowMessage("start: size=" + std::to_string(MPIWrapper::MPISize()) +
            ", n=" + std::to_string(runningWaveWithHighFrequency.nx) + ", guard=" + std::to_string(runningWaveWithHighFrequency.guard));
        DoConsistentPart();
        DoParallelPart();
    }

};