#include "mpi_worker.h"
#include "field_solver.h"
#include "fourier_transform.h"
#include "file_writer.h"
#include "simple_types.h"
#include "filter.h"

static int ch = 0;

void parallelSchemeOneExchange(MPIWorker& worker, const FieldSolver& fieldSolver, int numIter, double dt,
    FileWriter& fileWriter, bool writeFile) {
    worker.applyMask();

    fourierTransform(worker.getGrid(), RtoC);
    for (int i = 0; i < numIter; i++)
        fieldSolver(worker.getGrid(), dt);
    fourierTransform(worker.getGrid(), CtoR);

    if (writeFile)
        fileWriter.write(worker.getGrid(), "iter_rank_" + std::to_string(MPIWrapper::MPIRank()) +
            "_before_last_exc.csv", Double);
    worker.exchangeGuard();
    if (writeFile)
        fileWriter.write(worker.getGrid(), "iter_rank_" + std::to_string(MPIWrapper::MPIRank()) +
            "_after_last_exc.csv", Double);
}

void parallelScheme(MPIWorker& worker, const FieldSolver& fieldSolver, int numIter, int maxIterBetweenExchange,
    double dt, FileWriter& fileWriter) {
    if (numIter == 0 || maxIterBetweenExchange == 0) return;
    int numExchanges = numIter / maxIterBetweenExchange;
    int numIterBeforeLastExchange = numIter % maxIterBetweenExchange;

    for (int i = 0; i < numExchanges - 1; i++)
        parallelSchemeOneExchange(worker, fieldSolver, maxIterBetweenExchange, dt, fileWriter, false);

    if (numIterBeforeLastExchange != 0) {
        parallelSchemeOneExchange(worker, fieldSolver, maxIterBetweenExchange, dt, fileWriter, false);
        parallelSchemeOneExchange(worker, fieldSolver, numIterBeforeLastExchange, dt, fileWriter, true);
    }
    else {
        parallelSchemeOneExchange(worker, fieldSolver, maxIterBetweenExchange, dt, fileWriter, true);
    }
}