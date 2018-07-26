#pragma once
class Grid3d;
class MPIWorker;
#include <string>
#include "file_writer.h"

void FieldSolverPSTD(Grid3d& gr, double dt);
void FieldSolverPSATD(Grid3d& gr, double dt);

inline void FieldSolver(Grid3d& gr, double dt) {
    FieldSolverPSATD(gr, dt);
}
void FieldSolverParallel(MPIWorker& worker, int numIter, double dt, 
    int iterWriteFile, FileWriter& fileWriter);

