#pragma once
class Grid3d;
class MPIWorker;
#include <string>

void FieldSolverPSTD(Grid3d& gr, double dt);
void FieldSolverPSATD(Grid3d& gr, double dt);
void FieldSolverParallelPSATD(MPIWorker& worker, int numIter, double dt, int iterWriteFile, std::string dir);