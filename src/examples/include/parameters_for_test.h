#pragma once
#include "field_solver.h"
#include "filter.h"
#include "mask.h"
#include "vec.h"
#include "file_writer.h"
#include <iostream>
#include <string>
#include <cmath>

struct ParametersForTest {

};

struct ParametersForMyTest : public ParametersForTest {

    //метод
    FieldSolver fieldSolver;

    // сетка
    vec3<int> n;
    vec3<int> guard;

    vec3<double> d;    // шаг сетки
    vec3<double> a, b;    // координаты сетки

    // маска
    Mask mask;

    // фильтр
    Filter filter;

    // параметры счета
    int nSeqSteps;
    int nParSteps;
    int nDomainSteps;

    // вывод
    FileWriter fileWriter;

    double dt;

    ParametersForMyTest() :filter(vec3<int>(8), vec3<int>(2)),
        mask(SmoothMask), fieldSolver(PSATD) {
        mask.setMaskWidth(vec3<int>(8));
    }

    int getNSteps() {
        return nSeqSteps + nParSteps;
    }

    void print() {

		int numExchanges = nParSteps / nDomainSteps;
		int numIterBeforeLastExchange = nParSteps % nDomainSteps;

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
			"num of consistent steps = " << nSeqSteps << "\n" <<
			"num of parallel steps = " << nParSteps << "\n" <<
			"num of steps between exchanges = " << nDomainSteps << "\n" <<
			"number of exchanges = " << numExchanges + 1 << "\n" <<
		    "number of iterations before last exchange = " << numIterBeforeLastExchange << "\n";
    }
};