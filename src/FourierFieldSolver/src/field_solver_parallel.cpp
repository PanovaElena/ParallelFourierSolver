#include "mpi_worker.h"
#include "field_solver.h"
#include "fourier_transformation.h"
#include <fstream>
#include "file_writer.h"
#include "simple_types_and_constants.h"
#include <limits>

void WriteFile(Grid3d& gr, int iter, FileWriter& fileWriter) {
    FourierTransformation(gr, CtoR);
    fileWriter.WriteFile(gr, "iter_rank_" + std::to_string(MPIWrapper::MPIRank()) + ".csv");
}

void Dump(Grid3d& gr, int iter, FileWriter& fileWriter) {
    FourierTransformation(gr, CtoR);
    fileWriter.WriteFile(gr, "dump_iter_" + std::to_string(iter) + ".csv");
}

void FieldSolverParallelInnerCircle(MPIWorker& worker, FieldSolver fieldSolver, int numIter, double dt,
    int iterBetweenDumps, FileWriter& fileWriter) {
    FourierTransformation(worker.getGrid(), RtoC); //это нужно
    for (int i = 0; i < numIter; i++) {
        fieldSolver(worker.getGrid(), dt);
        // dump of every domain
        if (iterBetweenDumps !=0 && (i + 1) % iterBetweenDumps == 0)
            WriteFile(worker.getGrid(), i, fileWriter);
    }
    FourierTransformation(worker.getGrid(), CtoR);
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
        worker.ShowMessage("max iter between exchange - " + std::to_string(maxIterBetweenExchange));
        worker.ShowMessage("number of iter before last exchange - " + std::to_string(numIterBeforeLastExchange));
    }
}

void FieldSolverParallel(MPIWorker& worker, FieldSolver fieldSolver, int numIter, double dt,
    int iterBetweenDumps, FileWriter& fileWriter) {
    const double P = 0.8; // какую часть перекрытия волна должна пройти
    int maxIterBetweenExchange = (int) myRound(P*worker.getGuardSize()*worker.getGrid().gdx() / (constants::c*dt));
    int numExchanges = numIter / maxIterBetweenExchange;
    int numIterBeforeLastExchange = numIter % maxIterBetweenExchange;
    printInfo(worker, numExchanges, maxIterBetweenExchange, numIterBeforeLastExchange);
    for (int i = 0; i < numExchanges; i++) {
        FieldSolverParallelInnerCircle(worker, fieldSolver, maxIterBetweenExchange, dt, 0, fileWriter);
        worker.ApplyMask();
    }
    FieldSolverParallelInnerCircle(worker, fieldSolver, numIterBeforeLastExchange, dt,
        numIterBeforeLastExchange, fileWriter);
}