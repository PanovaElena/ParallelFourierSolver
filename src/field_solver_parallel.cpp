#include "field_solver.h"
#include "fourier_transformation.h"
#include "mpi_worker.h"
#include <fstream>
#include "write_file.h"
#include "simple_types.h"

void WriteFile(Grid3d& gr, int iter, std::string dir, int rank, TypeWriteFileField writeFile) {
    FourierTransformation(gr, CtoR);
    MPIWorker::ShowMessage("writing parallel result to file, iter=" + std::to_string(iter));
    writeFile(E, y, gr, dir + "/rank_" + std::to_string(rank) + "_iter_" + 
        std::to_string(iter) + ".csv");
}

void FieldSolverParallel(MPIWorker& worker, int numIter, double dt, 
    int iterWriteFile, std::string dir, TypeWriteFileField writeFile) {
    FourierTransformation(worker.getGrid(), RtoC);
    for (int i = 0; i < numIter; i++) {
        FieldSolver(worker.getGrid(), dt);
        if ((i + 1) % iterWriteFile == 0) 
            WriteFile(worker.getGrid(), i, dir, MPIWrapper::MPIRank(), writeFile);
    }
    FourierTransformation(worker.getGrid(), CtoR);
    worker.ExchangeGuard();
    worker.SetToZerosQuard();
}