#include "field_solver.h"
#include "fourier_transformation.h"
#include "mpi_worker.h"
#include <fstream>
#include "file_writer.h"
#include "simple_types_and_constants.h"

void WriteFile(Grid3d& gr, int iter, int rank, FileWriter& fileWriter) {
    FourierTransformation(gr, CtoR);
    //MPIWorker::ShowMessage("writing parallel result to file, iter=" + std::to_string(iter));
    fileWriter.WriteFile(gr, "iter_rank_" + std::to_string(rank) + ".csv");
}

void FieldSolverParallelInnerCircle(MPIWorker& worker, FieldSolverType fieldSolver, int numIter, double dt,
    int iterWriteFile, FileWriter& fileWriter) {
    FourierTransformation(worker.getGrid(), RtoC); //это нужно
    for (int i = 0; i < numIter; i++) {
        fieldSolver(worker.getGrid(), dt);
        if ((i + 1) % iterWriteFile == 0)
            WriteFile(worker.getGrid(), i, MPIWrapper::MPIRank(), fileWriter);
    }
    FourierTransformation(worker.getGrid(), CtoR);
    worker.ExchangeGuard();
}

void FieldSolverParallel(MPIWorker& worker, FieldSolverType fieldSolver, int numIter, double dt,
    int iterWriteFile, FileWriter& fileWriter) {
    int maxInnerIter = (int) ((worker.getGuardSize() - 8)*worker.getGrid().gdx() / (constants::c*dt));
    int numOuterIter = numIter / maxInnerIter;
    int numRemaindIter = numIter % maxInnerIter;
    if (MPIWrapper::MPIRank()==0)
        worker.ShowMessage("altogether it's " + std::to_string(numOuterIter + 1) + " inner circles");

    for (int i = 0; i < numOuterIter; i++) {
        //worker.ShowMessage(std::to_string(i+1)+" inner circle");
        FieldSolverParallelInnerCircle(worker, fieldSolver, maxInnerIter, dt, maxInnerIter, fileWriter);
        worker.ApplyMask();
    }
    FieldSolverParallelInnerCircle(worker, fieldSolver, numRemaindIter, dt, numRemaindIter, fileWriter);
}