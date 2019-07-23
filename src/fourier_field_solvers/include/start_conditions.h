#pragma once
#include "vector3d.h"
#include "field_solver.h"

class StartConditions {
public:
    vec3<double> a, d;
    double dt;
    FieldSolver fs;

    StartConditions() {}
    StartConditions(vec3<> _a, vec3<> _d, double _dt, const FieldSolver& _fs) :
        a(_a), d(_d), dt(_dt), fs(_fs) {}

    virtual vec3<double> fE(vec3<int> index) const { return vec3<>(0); }
    virtual vec3<double> fB(vec3<int> index) const { return vec3<>(0); }
    virtual vec3<double> fJ(vec3<int> index, int numIter) const { return vec3<>(0); }

    vec3<> getCoord(vec3<double> node) const {
        return a + (vec3<>)node * d;
    }
};