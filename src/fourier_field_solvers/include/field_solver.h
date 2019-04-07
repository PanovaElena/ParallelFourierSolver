#pragma once
#include <string>
#include <map>
#include "physical_constants.h"
#include "fourier_transformation.h"
#include "simple_types.h"
class Grid3d;
class MPIWorker;
class FileWriter;
class Filter;

#define COURANT_CONDITION_PSTD(d) (sqrt(2)*(d)/(constants::c*constants::pi))
#define COURANT_CONDITION_FDTD(d) ((d)/(constants::c*sqrt(2)))

typedef void(*FieldSolverType)(Grid3d&, double);

void fieldSolverFDTD(Grid3d& gr, double dt);
void fieldSolverPSTD(Grid3d& gr, double dt);
void fieldSolverPSATD(Grid3d& gr, double dt);

class FieldSolver {
    FieldSolverType func;
    std::string str;
    // смещения для начальных условий
    double displ_t_E, displ_t_B, displ_t_J;
    vec3<vec3<double>> displ_c_E, displ_c_B, displ_c_J;

public:
    FieldSolver(FieldSolverType f, std::string s, double _displ_t_E, double _displ_t_B, double _displ_t_J,
        vec3<vec3<double>> _displ_c_E, vec3<vec3<double>> _displ_c_B, vec3<vec3<double>> _displ_c_J) {
        func = f;
        str = s;
        displ_t_E = _displ_t_E;
        displ_t_B = _displ_t_B;
        displ_t_J = _displ_t_J;
        displ_c_E = _displ_c_E;
        displ_c_B = _displ_c_B;
        displ_c_J = _displ_c_J;
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
            return displ_c_E;
        case B:
            return displ_c_B;
        default:
            return displ_c_J;
        }
    }

    double getTimeOffset(Field field) {
        switch (field) {
        case E:
            return displ_t_E;
        case B:
            return displ_t_B;
        default:
            return displ_t_J;
        }
    }

    friend void transformGridIfNecessary(FieldSolver fs, Grid3d& gr, Direction dir) {
        if (fs.str == "FDTD") return;
        fourierTransformation(gr, dir);
    }

    friend void transformGridIfNecessary(FieldSolver fs, Grid3d& gr, Field f, Coordinate c, Direction dir) {
        if (fs.str == "FDTD") return;
        fourierTransformation(gr, f, c, dir);
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
    double dt, Filter& filter, FileWriter& fileWriter);

inline int mod(int a, int b) {
    return (a + b) % b;
}


