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
        a = vec3<double>(0); b = a + (vec3<double>)n * d;
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
        fileWriter.Initialize("./", E, z, Section(Section::XOY, Section::center));
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

    ParametersForSphericalWave parameters;
   
    Grid3d gr;

    SphericalWave() : parameters() {
        Initialize();
    }

    void SetParamsForTest(ParametersForSphericalWave p) {
        parameters = p;
        Initialize();
    }

    void Initialize() {
        gr = Grid3d(parameters.n, parameters.a, parameters.a + (vec3<double>)parameters.n*parameters.d);
        SetEB();
    }

    double GetJ(Grid3d& grid, int i, int j, int k, int iter) {
        vec3<vec3<double>> dcJ = parameters.fieldSolver.getCoordOffset(J);
        double dtJ = parameters.fieldSolver.getTimeOffset(J);
        vec3<double> coord(grid.getCoord(vec3<>(i + dcJ.z.x, j + dcJ.z.y, k + dcJ.z.z)));
        return parameters.source.GetJ(coord, (iter + dtJ)*parameters.dt);
    }

    void SetJ(int iter, Grid3d& grid) {
        for (int i = 0; i < grid.gnxRealNodes(); i++)
            for (int j = 0; j < grid.gnyRealNodes(); j++)
                for (int k = 0; k < grid.gnzRealNodes(); k++) {
                    double J0 = GetJ(grid, i, j, k, iter);
                    grid.J.Write(i, j, k, vec3<double>(0, 0, J0));
                }
    }

    virtual void SetEB() {
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            for (int j = 0; j < gr.gnyRealNodes(); j++)
                for (int k = 0; k < gr.gnzRealNodes(); k++) {
                    gr.E.Write(i, j, k, vec3<double>(0, 0, 0));
                    gr.B.Write(i, j, k, vec3<double>(0, 0, 0));
                }

        SetJ(0, gr);
    }

};

