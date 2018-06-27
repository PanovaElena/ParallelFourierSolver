#pragma once
class Grid3d;
class MPIWorker;
#include <string>
#include "write_file.h"

void FieldSolverPSTD(Grid3d& gr, double dt);
void FieldSolverPSATD(Grid3d& gr, double dt);

inline void FieldSolver(Grid3d& gr, double dt) {
    FieldSolverPSTD(gr, dt);
}
void FieldSolverParallel(MPIWorker& worker, int numIter, double dt, 
    int iterWriteFile, std::string dir, TypeWriteFileField writeFile);

