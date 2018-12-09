#include "mpi_worker.h"
#include "field_solver.h"
#include "fourier_transformation.h"
#include <fstream>
#include "file_writer.h"
#include "simple_types_and_constants.h"
#include <limits>
#include <algorithm>

void WriteFile(Grid3d& gr, int iter, FileWriter& fileWriter) {
    FourierTransformation(gr, CtoR);
    fileWriter.WriteFile(gr, "iter_rank_" + std::to_string(MPIWrapper::MPIRank()) + ".csv");
    FourierTransformation(gr, RtoC);
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

int getMaxIterBetweenExchange(MPIWorker& worker, double dt) {
    const double P = worker.getPartGuard(); // какую часть перекрытия волна должна пройти
    double gsx = worker.getGuardSize().get_x() == 0 ? std::numeric_limits<double>::max() : worker.getGuardSize().get_x()*worker.getGrid().gdx(),
        gsy = worker.getGuardSize().get_y() == 0 ? std::numeric_limits<double>::max() : worker.getGuardSize().get_y()*worker.getGrid().gdy(),
        gsz = worker.getGuardSize().get_z() == 0 ? std::numeric_limits<double>::max() : worker.getGuardSize().get_z()*worker.getGrid().gdz();
    double gs = std::min(gsx, std::min(gsy, gsz));
    return (int) myRound(P*gs / (constants::c*dt));
}

void FieldSolverParallel(MPIWorker& worker, FieldSolver fieldSolver, int numIter, double dt,
    int iterBetweenDumps, FileWriter& fileWriter) {
    int maxIterBetweenExchange = getMaxIterBetweenExchange(worker, dt);
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