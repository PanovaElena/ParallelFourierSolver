#pragma once
class Grid3d;
class MPIWorker;
#include <string>
#include "file_writer.h"
#include "cmath"
#include "physical_constants.h"

#define COURANT_CONDITION_PSTD(d) (sqrt(2)*(d)/(constants::c*constants::pi))

typedef void (*FieldSolverType)(Grid3d&, double);

void FieldSolverPSTD(Grid3d& gr, double dt);
void FieldSolverPSATD(Grid3d& gr, double dt);

void FieldSolverParallel(MPIWorker& worker, FieldSolverType fieldSolver, int numIter, double dt, 
    int iterWriteFile, FileWriter& fileWriter);

