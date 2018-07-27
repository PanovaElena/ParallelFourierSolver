#include "field_solver.h"
#include "fourier_transformation.h"
#include "mpi_worker.h"
#include <fstream>
#include "file_writer.h"
#include "simple_types_and_constants.h"

void WriteFile(Grid3d& gr, int iter, int rank, FileWriter& fileWriter) {
    FourierTransformation(gr, CtoR);
    MPIWorker::ShowMessage("writing parallel result to file, iter=" + std::to_string(iter));
    fileWriter.WriteFile(gr, "rank_" + std::to_string(rank) + "_iter_" + 
        std::to_string(iter) + ".csv");
}

void FieldSolverParallel(MPIWorker& worker, int numIter, double dt, 
    int iterWriteFile, FileWriter& fileWriter) {
    FourierTransformation(worker.getGrid(), RtoC); //это нужно
    for (int i = 0; i < numIter; i++) {
        FieldSolver(worker.getGrid(), dt);
        if ((i + 1) % iterWriteFile == 0) 
            WriteFile(worker.getGrid(), i, MPIWrapper::MPIRank(), fileWriter);
    }
    FourierTransformation(worker.getGrid(), CtoR);
    worker.ExchangeGuard();
}