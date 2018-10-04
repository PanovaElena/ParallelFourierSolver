#include "mpi_worker.h"
#include "field_solver.h"
#include "fourier_transformation.h"
#include <fstream>
#include "file_writer.h"
#include "simple_types_and_constants.h"

void WriteFile(Grid3d& gr, int iter, int rank, FileWriter& fileWriter) {
    FourierTransformation(gr, CtoR);
    //MPIWorker::ShowMessage("writing parallel result to file, iter=" + std::to_string(iter));
    fileWriter.WriteFile(gr, "iter_rank_" + std::to_string(rank) + ".csv");
}

void FieldSolverParallelInnerCircle(MPIWorker& worker, FieldSolverType fieldSolver, int numIter, double dt,
    int iterWriteFile, FileWriter& fileWriter, int I) {
    FourierTransformation(worker.getGrid(), RtoC); //это нужно
    for (int i = 0; i < numIter; i++) {
        fieldSolver(worker.getGrid(), dt);
        if (iterWriteFile!=0 && (i + 1) % iterWriteFile == 0)
            WriteFile(worker.getGrid(), i, MPIWrapper::MPIRank(), fileWriter);
    }
    FourierTransformation(worker.getGrid(), CtoR);
    worker.ExchangeGuard();
}

double round(double x, int signAfterDot) {
    return ((int)(x*pow(10, signAfterDot) + 0.5)) / pow(10, signAfterDot);
}

void FieldSolverParallel(MPIWorker& worker, FieldSolverType fieldSolver, int numIter, double dt,
    int iterWriteFile, FileWriter& fileWriter) {
    int maxInnerIter = (int) round(0.75*worker.getGuardSize()*worker.getGrid().gdx() / (constants::c*dt), 6);
    int numOuterIter = numIter / maxInnerIter;
    int numRemaindIter = numIter % maxInnerIter;
    if (MPIWrapper::MPIRank() == 0) {
        worker.ShowMessage("numOuterIter " + std::to_string(numOuterIter) + " inner circles");
        worker.ShowMessage("maxInnerIter " + std::to_string(maxInnerIter));
        worker.ShowMessage("numRemaindIter " + std::to_string(numRemaindIter));
    }

    for (int i = 0; i < numOuterIter; i++) {
        //worker.ShowMessage(std::to_string(i+1)+" inner circle");
        FieldSolverParallelInnerCircle(worker, fieldSolver, maxInnerIter, dt, 0, fileWriter, i+1);
        worker.ApplyMask();
    }
    FieldSolverParallelInnerCircle(worker, fieldSolver, numRemaindIter, dt, numRemaindIter, fileWriter, 0);
}