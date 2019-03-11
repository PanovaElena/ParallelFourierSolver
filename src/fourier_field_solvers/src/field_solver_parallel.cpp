#include "mpi_worker.h"
#include "field_solver.h"
#include "fourier_transformation.h"
#include "file_writer.h"
#include "simple_types_and_constants.h"
#include <limits>
#include <algorithm>

void FieldSolverParallelOneExchange(MPIWorker& worker, FieldSolver fieldSolver, int numIter, double dt,
    FileWriter& fileWriter) {
    FourierTransformation(worker.getGrid(), RtoC);
    for (int i = 0; i < numIter; i++) 
        fieldSolver(worker.getGrid(), dt);
    FourierTransformation(worker.getGrid(), CtoR);
    fileWriter.WriteFile(worker.getGrid(), "iter_rank_" + std::to_string(MPIWrapper::MPIRank()) + "_before_exc.csv", Double);
    worker.ExchangeGuard();
    fileWriter.WriteFile(worker.getGrid(), "iter_rank_" + std::to_string(MPIWrapper::MPIRank()) + "_after_exc.csv", Double);
}

void FieldSolverParallel(MPIWorker& worker, FieldSolver fieldSolver, int numIter, int maxIterBetweenExchange,
    double dt, FileWriter& fileWriter) {
    if (numIter == 0 || maxIterBetweenExchange == 0) return;
    int numExchanges = numIter / maxIterBetweenExchange;
    int numIterBeforeLastExchange = numIter % maxIterBetweenExchange;

    for (int i = 0; i < numExchanges; i++) {
        FieldSolverParallelOneExchange(worker, fieldSolver, maxIterBetweenExchange, dt, fileWriter);
        worker.ApplyMask();
    }
    FieldSolverParallelOneExchange(worker, fieldSolver, numIterBeforeLastExchange, dt, fileWriter);
}