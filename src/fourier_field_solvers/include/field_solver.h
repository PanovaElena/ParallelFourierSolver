#pragma once
#include <string>
#include <map>
#include "physical_constants.h"
#include "fourier_transform.h"
#include "simple_types.h"
class Grid3d;
class MPIWorker;
class FileWriter;

#define COURANT_CONDITION_PSTD(d) (sqrt(2)*(d)/(constants::c*constants::pi))
#define COURANT_CONDITION_FDTD(d) ((d)/(constants::c*sqrt(2)))

typedef void(*FieldSolverType)(Grid3d&, double);

void fieldSolverFDTD(Grid3d& gr, double dt);
void fieldSolverPSTD(Grid3d& gr, double dt);
void fieldSolverPSATD(Grid3d& gr, double dt);

class FieldSolver {
    FieldSolverType func;
    std::string str;
public:
    // смещения для начальных условий
    double shiftEt, shiftBt, shiftJt;
    vec3<vec3<double>> shiftE, shiftB, shiftJ;

    FieldSolver() {}
    FieldSolver(FieldSolverType f, std::string s, double _shiftEt, double _shiftBt, double _shiftJt,
        vec3<vec3<double>> _shiftE, vec3<vec3<double>> _shiftB, vec3<vec3<double>> _shiftJ) {
        func = f;
        str = s;
        shiftEt = _shiftEt;
        shiftBt = _shiftBt;
        shiftJt = _shiftJt;
        shiftE = _shiftE;
        shiftB = _shiftB;
        shiftJ = _shiftJ;
    }
    std::string to_string() {
        return str;
    }
    void operator() (Grid3d& gr, double dt) {
        func(gr, dt);
    }

    vec3<vec3<double>> getCoordOffset(Field field) {
        switch (field) {
        case E:
            return shiftE;
        case B:
            return shiftB;
        default:
            return shiftJ;
        }
    }

    double getTimeOffset(Field field) {
        switch (field) {
        case E:
            return shiftEt;
        case B:
            return shiftBt;
        default:
            return shiftJt;
        }
    }

    friend void transformGridIfNecessary(FieldSolver fs, Grid3d& gr,
        Direction dir, bool useMpi = false) {
        if (fs.str == "FDTD") return;
        fourierTransform(gr, dir, useMpi);
    }

    friend void transformGridIfNecessary(FieldSolver fs, Grid3d& gr, Field f, Coordinate c,
        Direction dir, bool useMpi = false) {
        if (fs.str == "FDTD") return;
        fourierTransform(gr, f, c, dir, useMpi);
    }
};

const FieldSolver PSTD(fieldSolverPSTD, "PSTD", 0, 0.5, 0.5,
    vec3<vec3<double>>(vec3<double>(0)),
    vec3<vec3<double>>(vec3<double>(0)),
    vec3<vec3<double>>(vec3<double>(0)));

const FieldSolver PSATD(fieldSolverPSATD, "PSATD", 0, 0, 0.5,
    vec3<vec3<double>>(vec3<double>(0)),
    vec3<vec3<double>>(vec3<double>(0)),
    vec3<vec3<double>>(vec3<double>(0)));

const FieldSolver FDTD(fieldSolverFDTD, "FDTD", 0, -0.5, 0.5,
    vec3<vec3<double>>(vec3<double>(0.5, 0, 0), vec3<double>(0, 0.5, 0), vec3<double>(0, 0, 0.5)),
    vec3<vec3<double>>(vec3<double>(0, 0.5, 0.5), vec3<double>(0.5, 0, 0.5), vec3<double>(0.5, 0.5, 0)),
    vec3<vec3<double>>(vec3<double>(0.5, 0, 0), vec3<double>(0, 0.5, 0), vec3<double>(0, 0, 0.5)));

const std::map<std::string, FieldSolver> FieldSolverMap =
{ { "PSTD",PSTD },{ "PSATD",PSATD },{ "FDTD",FDTD } };

void spectralSolverParallel(MPIWorker& worker, FieldSolver fieldSolver, int numIter, int maxNumIterBetweenExchanges,
    double dt, FileWriter& fileWriter);

inline int mod(int a, int b) {
    return (a + b) % b;
}


