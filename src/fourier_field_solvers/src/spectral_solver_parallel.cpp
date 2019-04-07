#include "mpi_worker.h"
#include "field_solver.h"
#include "fourier_transformation.h"
#include "file_writer.h"
#include "simple_types.h"
#include "filter.h"

void spectralSolverParallelOneExchange(MPIWorker& worker, FieldSolver fieldSolver, int numIter, double dt,
    int nExchange, Filter& filter, FileWriter& fileWriter) {
    fourierTransformation(worker.getGrid(), RtoC);

    if (nExchange != 0)
        filter(worker.getGrid());
    for (int i = 0; i < numIter; i++)
        fieldSolver(worker.getGrid(), dt);

    fourierTransformation(worker.getGrid(), CtoR);

    fileWriter.write(worker.getGrid(), "iter_rank_" + std::to_string(MPIWrapper::MPIRank()) + "_before_exc.csv", Double);
    worker.ExchangeGuard();
    fileWriter.write(worker.getGrid(), "iter_rank_" + std::to_string(MPIWrapper::MPIRank()) + "_after_exc.csv", Double);
}

void spectralSolverParallel(MPIWorker& worker, FieldSolver fieldSolver, int numIter, int maxIterBetweenExchange,
    double dt, Filter& filter, FileWriter& fileWriter) {
    if (numIter == 0 || maxIterBetweenExchange == 0) return;
    int numExchanges = numIter / maxIterBetweenExchange;
    int numIterBeforeLastExchange = numIter % maxIterBetweenExchange;

    for (int i = 0; i < numExchanges; i++) {
        worker.applyMask();
        spectralSolverParallelOneExchange(worker, fieldSolver, maxIterBetweenExchange, dt,
            i, filter, fileWriter);
    }
    if (numIterBeforeLastExchange != 0) {
        worker.applyMask();
        spectralSolverParallelOneExchange(worker, fieldSolver, numIterBeforeLastExchange, dt,
            numExchanges, filter, fileWriter);
    }

    filter(worker.getGrid());
}