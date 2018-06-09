#include "field_solver.h"
#include "fourier_transformation.h"
#include "mpi_worker.h"
#include <fstream>
#include "write_file.h"

void WriteFile(Grid3d& gr, int iter, std::string dir, int rank) {
    FourierTransformation(gr, CtoR);
    WriteFileField(E, z, gr, dir + "/rank_" + std::to_string(rank) + "_iter_" + std::to_string(iter) + "_E.csv");
}

void FieldSolverParallelPSATD(MPIWorker& worker, int numIter, double dt, int iterWriteFile, std::string dir) {
    FourierTransformation(worker.getGrid(), RtoC);
    for (int i = 0; i < numIter; i++) {
        FieldSolverPSATD(worker.getGrid(), dt);
        if ((i + 1) % iterWriteFile == 0) {
            MPIWorker::ShowMessage("write to file parallel result before setting to zeros guard");
            WriteFile(worker.getGrid(), i, dir, MPIWrapper::MPIRank());
        }
    }
    FourierTransformation(worker.getGrid(), CtoR);
    worker.ExchangeGuard();
    worker.SetToZerosQuard();
}