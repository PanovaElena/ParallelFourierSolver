#include "field_solver.h"
#include "fourier_transformation.h"
#include "mpi_worker.h"

void FieldSolverParallelPSATD(MPIWorker& worker, int numIter, double dt) {
    for (int i = 0; i < numIter; i++)
        FieldSolverPSATD(worker.getGrid(), dt);
    FourierTransformation(worker.getGrid(), CtoR);
    MPIWorker::ShowMessage("parallel: exchange");
    worker.ExchangeGuard();
    MPIWorker::ShowMessage("parallel: setting to zeros");
    worker.SetToZerosQuard();
    MPIWorker::ShowMessage("parallel: fourier RtoC");
    FourierTransformation(worker.getGrid(), RtoC);
}