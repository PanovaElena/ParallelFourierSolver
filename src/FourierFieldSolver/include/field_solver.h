#pragma once
class Grid3d;
class MPIWorker;
#include <string>
#include "file_writer.h"
#include "cmath"
#include "physical_constants.h"
#include <map>
#include <string>
#include "fourier_transformation.h"
#include "simple_types_and_constants.h"

#define COURANT_CONDITION_PSTD(d) (sqrt(2)*(d)/(constants::c*constants::pi))
#define COURANT_CONDITION_FDTD(d) ((d)/(constants::c*sqrt(2)))

typedef void (*FieldSolverType)(Grid3d&, double);

void fieldSolverFDTD(Grid3d& gr, double dt);
void fieldSolverPSTD(Grid3d& gr, double dt);
void fieldSolverPSATD(Grid3d& gr, double dt);
void fieldSolverPSATD_omp(Grid3d& gr, double dt);

class FieldSolver {
    FieldSolverType func;
    std::string str;
public:
    FieldSolver(FieldSolverType f, std::string s) {
        func = f;
        str = s;
    }
    std::string to_string() {
        return str;
    }
    void operator() (Grid3d& gr, double dt) {
        func(gr, dt);
    }

    friend void TransformGridIfNecessary(FieldSolver fs, Grid3d& gr, Direction dir) {
        if (fs.str == "FDTD") return;
        FourierTransformation(gr, dir);
    }

    friend void TransformGridIfNecessary(FieldSolver fs, Grid3d& gr, Field f, Coordinate c, Direction dir) {
        if (fs.str == "FDTD") return;
        FourierTransformation(gr, f, c, dir);
    }
};

const FieldSolver PSTD(fieldSolverPSTD, "PSTD");
const FieldSolver PSATD(fieldSolverPSATD, "PSATD");
const FieldSolver FDTD(fieldSolverFDTD, "FDTD");
const FieldSolver PSATD_OMP(fieldSolverPSATD_omp, "PSATD_omp");

const std::map<std::string, FieldSolver> FieldSolverMap =
    { { "PSTD",PSTD },{ "PSATD",PSATD },{ "FDTD",FDTD },{ "PSATD_omp",PSATD_OMP } };

void FieldSolverParallel(MPIWorker& worker, FieldSolver fieldSolver, int numIter, int maxNumIterBetweenExchanges,
    double dt, FileWriter& fileWriter);

inline int mod(int a, int b) {
    return (a + b) % b;
}


