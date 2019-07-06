#include "mpi_worker.h"
#include "field_solver.h"
#include "fourier_transform.h"
#include "file_writer.h"
#include "simple_types.h"

void spectralSolverParallelOneExchange(MPIWorker& worker, FieldSolver fieldSolver, int numIter, double dt,
    FileWriter& fileWriter) {
    worker.applyMask();

    fourierTransform(worker.getGrid(), RtoC);
    for (int i = 0; i < numIter; i++)
        fieldSolver(worker.getGrid(), dt);
    fourierTransform(worker.getGrid(), CtoR);

    fileWriter.write(worker.getGrid(), "iter_rank_" + std::to_string(MPIWrapper::MPIRank()) + "_before_exc.csv", Double);
    worker.ExchangeGuard();
    fileWriter.write(worker.getGrid(), "iter_rank_" + std::to_string(MPIWrapper::MPIRank()) + "_after_exc.csv", Double);
}

void spectralSolverParallel(MPIWorker& worker, FieldSolver fieldSolver, int numIter, int maxIterBetweenExchange,
    double dt, FileWriter& fileWriter) {
    if (numIter == 0 || maxIterBetweenExchange == 0) return;
    int numExchanges = numIter / maxIterBetweenExchange;
    int numIterBeforeLastExchange = numIter % maxIterBetweenExchange;

    for (int i = 0; i < numExchanges; i++)
        spectralSolverParallelOneExchange(worker, fieldSolver, maxIterBetweenExchange, dt, fileWriter);
    if (numIterBeforeLastExchange != 0)
        spectralSolverParallelOneExchange(worker, fieldSolver, numIterBeforeLastExchange, dt, fileWriter);
}