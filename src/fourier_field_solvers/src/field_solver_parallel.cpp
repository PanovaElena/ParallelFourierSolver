#include "mpi_worker.h"
#include "field_solver.h"
#include "fourier_transformation.h"
#include <fstream>
#include "file_writer.h"
#include "simple_types_and_constants.h"
#include <limits>
#include <algorithm>

void WriteFile(Grid3d& gr, int iter, FileWriter& fileWriter) {
    fileWriter.WriteFile(gr, "iter_rank_" + std::to_string(MPIWrapper::MPIRank()) + ".csv", Double);
}

void FieldSolverParallelInnerCircle(MPIWorker& worker, FieldSolver fieldSolver, int numIter, double dt,
    FileWriter& fileWriter) {
    FourierTransformation(worker.getGrid(), RtoC);
    for (int i = 0; i < numIter; i++) 
        fieldSolver(worker.getGrid(), dt);
    FourierTransformation(worker.getGrid(), CtoR);
    WriteFile(worker.getGrid(), numIter, fileWriter);
    worker.ExchangeGuard();
}

double myRound(double x) {
    if (x >= double(std::numeric_limits<int>::max()))
        return std::numeric_limits<int>::max();
    return (int)(x + 0.5);
}

void printInfo(MPIWorker& worker, int numExchanges, int maxIterBetweenExchange, int numIterBeforeLastExchange) {
    if (MPIWrapper::MPIRank() == 0) {
        worker.ShowMessage("number of exchanges - " + std::to_string(numExchanges + 1));
        worker.ShowMessage("max iter between exchanges - " + std::to_string(maxIterBetweenExchange));
        worker.ShowMessage("number of iter before last exchange - " + std::to_string(numIterBeforeLastExchange));
    }
}

void FieldSolverParallel(MPIWorker& worker, FieldSolver fieldSolver, int numIter, int maxIterBetweenExchange,
    double dt, FileWriter& fileWriter) {
    int numExchanges = numIter / maxIterBetweenExchange;
    int numIterBeforeLastExchange = numIter % maxIterBetweenExchange;
    printInfo(worker, numExchanges, maxIterBetweenExchange, numIterBeforeLastExchange);

    for (int i = 0; i < numExchanges; i++) {
        FieldSolverParallelInnerCircle(worker, fieldSolver, maxIterBetweenExchange, dt, fileWriter);
        worker.ApplyMask();
    }
    FieldSolverParallelInnerCircle(worker, fieldSolver, numIterBeforeLastExchange, dt, fileWriter);
}