#pragma once
#include "field_solver.h"

inline void FieldSolver(Grid3d& gr, double dt) {
    FieldSolverPSATD(gr, dt);
}