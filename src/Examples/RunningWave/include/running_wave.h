#pragma once
#include "grid3d.h"
#include "physical_constants.h"
#include "simple_types_and_constants.h"
#include "fourier_transformation.h"
#include "field_solver.h"
#include "class_member_ptr.h"
#include "file_writer.h"
#include "masks.h"


class RunningWave {
public:

    // вывод
    std::string dir = "../../../files/running_wave/";
    Section section;
    FileWriter fileWriter;

    //метод
    FieldSolverType fieldSolver = FieldSolverPSATD;

    // сетка
    int nx = 256, ny = nx, nz = 1;
    int guard = 64;

    Mask mask;
    int maskWidth = 8;

    double d = sqrt(6)*constants::pi;    // шаг сетки
    double a = 0, b = nx*d;    

    // физические параметры
    double lambda = 8 * d;

    // параметры счета
    int nStartSteps = 200;
    int nNextSteps = 600;

    int maxIt = nStartSteps + nNextSteps;
    int itTransform = 100;

    double dt;


    Grid3d gr;

    RunningWave() :gr(nx, ny, nz, a, b, a, b, a, a+d),   
        section(Section::XOY, Section::center, Section::XOZ, Section::center) {
        mask = maskSineSquare;
        fileWriter.Initialize(dir, E, y, section);
        dt = COURANT_CONDITION_PSTD(d);
        SetEB();
    }

    virtual double funcB(double x) {
        return sin(2 * constants::pi / lambda*x);
    }

    virtual double funcE(double x) {
        return sin(2 * constants::pi / lambda*x);
    }

    virtual void SetEB() {
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            for (int j = 0; j < gr.gnyRealNodes(); j++)
                for (int k = 0; k < gr.gnzRealNodes(); k++) {
                    gr(i, j, k).E = vec3<double>(0, 1, 0)*funcE(i*d + a);
                    gr(i, j, k).B = vec3<double>(0, 0, 1)*funcB(i*d + a);
                }

        FourierTransformation(gr, RtoC);
    }

    void SetNIterInPeriod(double _n_periods, double _n_steps_in_period) {
        dt = lambda / (constants::c*_n_steps_in_period);
        nStartSteps = 0;
        nNextSteps = (int) (_n_steps_in_period*_n_periods);
        maxIt = nStartSteps + nNextSteps;
    }

    //T=dt*n, n - целое
    void SetParamsForTest(double _lambda, FieldSolverType _fieldSolver, 
        double _n_periods = -1, double _n_steps_in_period = -1) {
        lambda = _lambda;
        fieldSolver = _fieldSolver;
        SetEB();
        if (_n_periods < 0) return;
        if (_n_steps_in_period < 0)
            _n_steps_in_period = (int)(_lambda / (constants::c*COURANT_CONDITION_PSTD(d))) + 1;
        SetNIterInPeriod(_n_periods, _n_steps_in_period);
    }
};

