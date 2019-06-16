#pragma once
#include <iostream>
#include <cmath>
#include "grid3d.h"
#include "physical_constants.h"
#include "simple_types.h"
#include "fourier_transformation.h"
#include "field_solver.h"
#include "class_member_ptr.h"
#include "file_writer.h"
#include "mask.h"
#include "filter.h"
#include "parameters_for_test.h"

struct ParametersForRunningWave : public ParametersForMyTest {

    // физические параметры
    double lambda;
    double angle;

    // параметры вывода
    int dimensionOfOutputData;

    ParametersForRunningWave() {
        n.x = 256; n.y = 1; n.z = n.x;
        guard = vec3<int>(64);
        d = vec3<double>(1);
        a = vec3<double>(0); b = vec3<double>(n.x*d.x);
        b = a + (vec3<double>)n * d;
        dt = 1 / (constants::c * 4);// COURANT_CONDITION_PSTD(fmin(fmin(d.x, d.y), d.z)) / 2;
        nSeqSteps = 200;
        nParSteps = 600;
        nDomainSteps = (int)(0.4*guard.x*d.x / constants::c / dt);
        lambda = 16 * d.x;
        angle = 0;
        dimensionOfOutputData = 1;
        fileWriter.initialize("./", E, y, Section(Section::XOY, Section::center, Section::XOZ, Section::center));
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
        initialize();
    }

    void setParamsForTest(ParametersForRunningWave p) {
        parameters = p;
        if (parameters.dimensionOfOutputData == 2)
            parameters.fileWriter.setSection(Section(Section::XOZ, Section::center));
        initialize();
    }

    void initialize() {
        gr = Grid3d(parameters.n, parameters.a, parameters.a + (vec3<double>)parameters.n*parameters.d);
        setEB();
    }

    double f(double x, double z, double t) {
        double x2 = x * cos(parameters.angle) + z * sin(parameters.angle);
        return sin(2 * constants::pi / parameters.lambda*(x2 - constants::c*t));
    }

    virtual void setEB() {

        vec3<vec3<double>> dcE = parameters.fieldSolver.getCoordOffset(E),
            dcB = parameters.fieldSolver.getCoordOffset(B);
        double dtE = parameters.fieldSolver.getTimeOffset(E),
            dtB = parameters.fieldSolver.getTimeOffset(B);

        for (int i = 0; i < gr.sizeReal().x; i++)
            for (int j = 0; j < gr.sizeReal().y; j++)
                for (int k = 0; k < gr.sizeReal().z; k++) {

                    double xEy = gr.getCoord(vec3<int>(i + dcE.y.x, 0, 0)).x,
                        zEy = (vec3<int>(0, 0, k + dcE.y.z)).z,
                        tE = dtE * parameters.dt;
                    double xBx = gr.getCoord(vec3<int>(i + dcB.x.x, 0, 0)).x,
                        zBx = (vec3<int>(0, 0, k + dcB.x.z)).z,
                        xBz = gr.getCoord(vec3<int>(i + dcB.z.x, 0, 0)).x,
                        zBz = (vec3<int>(0, 0, k + k + dcB.z.z)).z,
                        tB = dtB * parameters.dt;

                    gr.E.write(i, j, k, vec3<double>(0, f(xEy, zEy, tE), 0));
                    gr.B.write(i, j, k, vec3<double>(-sin(parameters.angle)*f(xBx, zBx, tB), 0,
                        cos(parameters.angle)*f(xBz, zBz, tB)));
                }

    }
};
