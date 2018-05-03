#include "field_solver.h"
#include "fourier_transformation.h"
#include "mpi_worker.h"
#include <fstream>

void FieldSolverParallelPSATD(MPIWorker& worker, int numIter, double dt) {
    FourierTransformation(worker.getGrid(), RtoC);
    for (int i = 0; i < numIter; i++)
        FieldSolverPSATD(worker.getGrid(), dt);
    FourierTransformation(worker.getGrid(), CtoR);
    worker.ExchangeGuard();
    worker.SetToZerosQuard();
    FourierTransformation(worker.getGrid(), RtoC);
}