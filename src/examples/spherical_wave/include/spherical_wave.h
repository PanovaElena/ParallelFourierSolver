#pragma once
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include "grid3d.h"
#include "class_member_ptr.h"
#include "file_writer.h"
#include "mask.h"
#include "filter.h"
#include "physical_constants.h"
#include "field_solver.h"
#include "parameters_for_test.h"
#include "source.h"

struct ParametersForSphericalWave : public ParametersForMyTest {

    // физические параметры
    Source source;

    ParametersForSphericalWave() {
        n.x = 128; n.y = n.x; n.z = 1;
        guard = vec3<int>(32);
        d = vec3<double>(constants::c);
        a = vec3<>(-1 * (vec3<>)(n / 2) * d);
        b = a + (vec3<>)n * d;
        dt = 0.1;
        nSeqSteps = 300;
        nParSteps = 100;
        nDomainSteps = (int)(0.4*guard.x*d.x / constants::c / dt);
        source.time = 16;
        source.omega = 2 * constants::pi / source.time;
        source.omegaEnv = source.omega;
        source.width = vec3<double>(d.x * 8, d.y * 8, d.z / 4);
        source.coord = vec3<double>(0, 0, 0);
        source.startTime = 0;
        fileWriter.initialize("./", E, z, Section(Section::XOY, Section::center));
    }

    void print() {
        ParametersForMyTest::print();
        std::cout <<
            "coordinate of source = " << source.coord << "\n" <<
            "omega = " << source.omega << "\n" <<
            "omega of envelope = " << source.omegaEnv << "\n" <<
            "time of working of sourse = " << source.time << "\n" <<
            "width of sourse = " << source.width << "\n" <<
            "start time of sourse = " << source.startTime << "\n" <<
            std::endl;
    }
};

class SphericalWave {
public:

    ParametersForSphericalWave params;

    Grid3d gr;

    SphericalWave() : params() {
        initialize();
    }

    void setParamsForTest(ParametersForSphericalWave p) {
        params = p;
        initialize();
    }

    void initialize() {
        gr = Grid3d(params.n, params.a, params.a + (vec3<double>)params.n*params.d);
        setEB();
    }

    double getJ(Grid3d& grid, int i, int j, int k, int iter) {
        vec3<vec3<double>> dcJ = params.fieldSolver.getCoordOffset(J);
        double dtJ = params.fieldSolver.getTimeOffset(J);
        vec3<double> coord(grid.getCoord(vec3<>(i + dcJ.z.x, j + dcJ.z.y, k + dcJ.z.z)));
        return params.source.getJ(coord, (iter + dtJ)*params.dt);
    }

    void SetJ(int iter, Grid3d& grid) {
        for (int i = 0; i < grid.sizeReal().x; i++)
            for (int j = 0; j < grid.sizeReal().y; j++)
                for (int k = 0; k < grid.sizeReal().z; k++) {
                    double J0 = getJ(grid, i, j, k, iter);
                    grid.J.write(i, j, k, vec3<double>(0, 0, J0));
                }
    }

    virtual void setEB() {
        for (int i = 0; i < gr.sizeReal().x; i++)
            for (int j = 0; j < gr.sizeReal().y; j++)
                for (int k = 0; k < gr.sizeReal().z; k++) {
                    gr.E.write(i, j, k, vec3<double>(0, 0, 0));
                    gr.B.write(i, j, k, vec3<double>(0, 0, 0));
                }

        SetJ(0, gr);
    }

};

