#pragma once
#include "field_solver.h"
#include "filter.h"
#include "mask.h"
#include "vec.h"
#include <iostream>

struct ParametersForTest {

};

struct ParametersForMyTest : public ParametersForTest {
    //метод
    FieldSolver fieldSolver;

    // сетка
    vec3<int> n;
    vec3<int> guard;

    double d;    // шаг сетки
    double a, b;    // координаты сетки

    // маска
    Mask mask;

    // фильтр
    Filter filter;

    // параметры счета
    int nConsSteps;
    int nParSteps;

    int nIterBetweenDumps;

    double dt;

    ParametersForMyTest() :filter(vec3<int>(8), vec3<int>(2)),
        mask(SmoothMask), fieldSolver(PSATD) {
        mask.setMaskWidth(vec3<int>(8));
    }

    int getNSteps() {
        return nConsSteps + nParSteps;
    }

    void print() {
        std::cout <<
            "field solver: " << fieldSolver.to_string() << "\n" <<
            "dt = " << dt << "\n" <<
            "n = " << n << "\n" <<
            "d = " << d << "\n" <<
            "guard = " << guard << "\n" <<
            "mask = " << mask.to_string() << "\n" <<
            "mask width = " << mask.getMaskWidth() << "\n" <<
            "filter = " << filter.to_string() << "\n" <<
            "filter width = " << filter.getWidth() << "\n" <<
            "num zero freq for filter = " << filter.getNumZeroFreq() << "\n" <<
            "num of steps = " << getNSteps() << "\n" <<
            "num of consistent steps = " << nConsSteps << "\n" <<
            "num of parallel steps = " << nParSteps << "\n";
    }
};