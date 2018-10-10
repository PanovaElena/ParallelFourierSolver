#pragma once
#include <iostream>
#include "grid3d.h"
#include "physical_constants.h"
#include "simple_types_and_constants.h"
#include "fourier_transformation.h"
#include "field_solver.h"
#include "class_member_ptr.h"
#include "file_writer.h"
#include "masks.h"
#include "parameters_for_test.h"

struct ParametersForRunningWave : public ParametersForTest {
    //метод
    FieldSolverType fieldSolver = FieldSolverPSATD;

    // сетка
    int nx = 256, ny = nx, nz = 1;
    int guard = 64;

    double d = sqrt(6)*constants::pi;    // шаг сетки
    double a = 0, b = nx*d;

    // маска
    Mask mask = maskSineSquare;
    int maskWidth = 8;

    // физические параметры
    double lambda = 8 * d;

    // параметры счета
    int nConsSteps = 200;
    int nParSteps = 600;

    int nIterBetweenDumps = 100;

    double dt;

    ParametersForRunningWave() : dt(COURANT_CONDITION_PSTD(d)) {
    }

    int getNSteps() {
        return nConsSteps + nParSteps;
    }

    void print() {
        std::cout <<
            "lambda = " << lambda << "\n" <<
            "dt = " << dt << "\n" <<
            "nx = " << nx << "\n" <<
            "ny = " << ny << "\n" <<
            "nz = " << nz << "\n" <<
            "d = " << d << "\n" <<
            "guard = " << guard << "\n" <<
            "num of steps = " << getNSteps() << "\n" <<
            "num of consistent steps = " << nConsSteps << "\n" <<
            "num of parallel steps = " << nParSteps << "\n" <<
            std::endl;
    }
};


class RunningWave {
public:

    // вывод
    std::string dir = std::string(ROOT_DIR) + "/files/running_wave/";
    Section section;
    FileWriter fileWriter;

    ParametersForRunningWave parameters;

    Grid3d gr;

    RunningWave() : parameters(), section(Section::XOY, Section::center, Section::XOZ, Section::center) {
        fileWriter.Initialize(dir, E, y, section);
        Initialize();
    }

    void SetParamsForTest(ParametersForRunningWave p) {
        parameters = p;
        Initialize();
    }

    void Initialize() {
        gr = Grid3d(parameters.nx, parameters.ny, parameters.nz, parameters.a, parameters.b, parameters.a, parameters.b, parameters.a, parameters.a + parameters.d);
        SetEB();
    }

    virtual double funcB(double x) {
        return sin(2 * constants::pi / parameters.lambda*x);
    }

    virtual double funcE(double x) {
        return sin(2 * constants::pi / parameters.lambda*x);
    }

    virtual void SetEB() {
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            for (int j = 0; j < gr.gnyRealNodes(); j++)
                for (int k = 0; k < gr.gnzRealNodes(); k++) {
                    gr(i, j, k).E = vec3<double>(0, 1, 0)*funcE(i*parameters.d + parameters.a);
                    gr(i, j, k).B = vec3<double>(0, 0, 1)*funcB(i*parameters.d + parameters.a);
                }

        FourierTransformation(gr, RtoC);
    }
};

