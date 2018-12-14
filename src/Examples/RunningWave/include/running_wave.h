#pragma once
#include <iostream>
#include "grid3d.h"
#include "physical_constants.h"
#include "simple_types_and_constants.h"
#include "fourier_transformation.h"
#include "field_solver.h"
#include "class_member_ptr.h"
#include "file_writer.h"
#include "mask.h"
#include "filter.h"
#include "parameters_for_test.h"

struct ParametersForRunningWave : public ParametersForTest {
    //метод
    FieldSolver fieldSolver = PSATD;

    // сетка
    int nx = 256, ny = 1, nz = nx;
    int guard = 64;

    double d = sqrt(6)*constants::pi;    // шаг сетки
    double a = 0, b = nx*d;

    // маска
    Mask mask = MaskSimple;
    int maskWidth = 8;

    // фильтр
    Filter filter;

    // физические параметры
    double lambda = 8 * d;
    double angle = 0;

    // параметры счета
    int nConsSteps = 200;
    int nParSteps = 600;

    int nIterBetweenDumps;
    int dimensionOfOutputData = 1;

    double dt;

    ParametersForRunningWave() {
        dt = COURANT_CONDITION_PSTD(d) / 2;
        nIterBetweenDumps = nParSteps;
    }

    int getNSteps() {
        return nConsSteps + nParSteps;
    }

    void print() {
        std::cout <<
            "field solver: " << fieldSolver.to_string() << "\n" <<
            "lambda = " << lambda << "\n" <<
            "angle = " << angle << "\n" <<
            "dt = " << dt << "\n" <<
            "nx = " << nx << "\n" <<
            "ny = " << ny << "\n" <<
            "nz = " << nz << "\n" <<
            "d = " << d << "\n" <<
            "guard = " << guard << "\n" <<
            "mask = " << mask.to_string() << "\n" <<
            "filter = " << filter.to_string() << "\n" <<
            "num of steps = " << getNSteps() << "\n" <<
            "num of consistent steps = " << nConsSteps << "\n" <<
            "num of parallel steps = " << nParSteps << "\n" <<
            "dimension of output data = " << dimensionOfOutputData << "\n" <<
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
        if (parameters.dimensionOfOutputData == 2) 
            fileWriter.SetSection(Section(Section::XOZ, Section::center));
        Initialize();
    }

    void Initialize() {
        gr = Grid3d(parameters.nx, parameters.ny, parameters.nz,
            parameters.a, parameters.a + parameters.nx*parameters.d,
            parameters.a, parameters.a + parameters.ny*parameters.d,
            parameters.a, parameters.a + parameters.nz*parameters.d);
        SetEB();
    }

    double f(double x) {
        return sin(2 * constants::pi / parameters.lambda*x);
    }

    virtual double funcB(double x, double z) {
        double x2 = x*cos(parameters.angle) + z*sin(parameters.angle);
        return f(x2);
    }

    virtual double funcE(double x, double z) {
        double x2 = x*cos(parameters.angle) + z*sin(parameters.angle);
        return f(x2);
    }

    virtual void SetEB() {
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            for (int j = 0; j < gr.gnyRealNodes(); j++)
                for (int k = 0; k < gr.gnzRealNodes(); k++) {
                    gr.E.Write(i, j, k, vec3<double>(0, 1, 0)
                        *funcE(i*parameters.d + parameters.a, k*parameters.d + parameters.a));
                    gr.B.Write(i, j, k, vec3<double>(-sin(parameters.angle), 0, cos(parameters.angle))
                        *funcB(i*parameters.d + parameters.a, k*parameters.d + parameters.a));
                }

        //TransformGridIfNecessary(parameters.fieldSolver, gr, RtoC);
    }
};
