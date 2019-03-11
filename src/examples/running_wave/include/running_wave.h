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
#include "cmath"

struct ParametersForRunningWave : public ParametersForMyTest {

    // физические параметры
    double lambda;
    double angle;

    // параметры вывода
    int dimensionOfOutputData;

    ParametersForRunningWave() {
        n.x = 256; n.y = 1; n.z = n.x;
        guard = vec3<int>(64);
        d = vec3<double>(sqrt(6)*constants::pi);
        a = vec3<double>(0); b = vec3<double>(n.x*d.x);
        b = a + (vec3<double>)n * d;
        dt = COURANT_CONDITION_PSTD(fmin(fmin(d.x, d.y), d.z)) / 2;
        nSeqSteps = 200;
        nParSteps = 600;
        nDomainSteps = (int) (0.4*guard.x*d.x / constants::c / dt);
        lambda = 8 * d.x;
        angle = 0;
        dimensionOfOutputData = 1;
        fileWriter.Initialize("./", E, y, Section(Section::XOY, Section::center, Section::XOZ, Section::center));
    }

    void print() {
        ParametersForMyTest::print();
        std::cout <<
            "lambda = " << lambda << "\n" <<
            "angle = " << angle << "\n" <<
            "dimension of output data = " << dimensionOfOutputData << "\n" <<
            std::endl;
    }
};


class RunningWave {
public:

    ParametersForRunningWave parameters;

    Grid3d gr;

    RunningWave() : parameters() {
        Initialize();
    }

    void SetParamsForTest(ParametersForRunningWave p) {
        parameters = p;
        if (parameters.dimensionOfOutputData == 2)
            parameters.fileWriter.SetSection(Section(Section::XOZ, Section::center));
        Initialize();
    }

    void Initialize() {
        gr = Grid3d(parameters.n, parameters.a, parameters.a + (vec3<double>)parameters.n*parameters.d);
        SetEB();
    }

    double f(double x, double z, double t) {
        double x2 = x*cos(parameters.angle) + z*sin(parameters.angle);
        return sin(2 * constants::pi / parameters.lambda*(x2 - constants::c*t));
    }

    double GetX(double i) {
        return i*parameters.d.x + parameters.a.x;
    }

    double GetY(double j) {
        return j*parameters.d.y + parameters.a.y;
    }

    double GetZ(double k) {
        return k*parameters.d.z + parameters.a.z;
    }

    virtual void SetEB() {

        vec3<vec3<double>> dcE = parameters.fieldSolver.getCoordOffset(E),
            dcB = parameters.fieldSolver.getCoordOffset(B);
        double dtE = parameters.fieldSolver.getTimeOffset(E),
            dtB = parameters.fieldSolver.getTimeOffset(B);

        for (int i = 0; i < gr.gnxRealNodes(); i++)
            for (int j = 0; j < gr.gnyRealNodes(); j++)
                for (int k = 0; k < gr.gnzRealNodes(); k++) {

                    double xEy = GetX(i + dcE.y.x), zEy = GetZ(k + dcE.y.z),
                        tE = dtE*parameters.dt;
                    double xBx = GetX(i + dcB.x.x), zBx = GetZ(k + dcB.x.z),
                        xBz = GetX(i + dcB.z.x), zBz = GetZ(k + dcB.z.z),
                        tB = dtB*parameters.dt;

                    gr.E.Write(i, j, k, vec3<double>(0, f(xEy, zEy, tE), 0));
                    gr.B.Write(i, j, k, vec3<double>(-sin(parameters.angle)*f(xBx, zBx, tB), 0,
                        cos(parameters.angle)*f(xBz, zBz, tB)));
                }

    }
};
