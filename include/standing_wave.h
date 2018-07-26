#pragma once
#include "grid3d.h"
#include "constants.h"
#include "simple_types.h"
#include "fourier_transformation.h"
#include "field_solver.h"
#include "class_member_ptr.h"
#include "write_file.h"


class StandingWave {
public:

    std::string dir = "../../../files/standing_wave/";

    TypeWriteFileField writeFile = WriteFileField2d;

    int NStartSteps = 600;
    int NNextSteps = 200;

    int maxIt = NStartSteps + NNextSteps;
    int itTransform = 100;

    int nx = 128, ny = nx, nz = 1;
    int guard = 64;

    double a = 0, b = nx;    // координаты сетки
    double d = 1;    // шаг сетки

    double dt;

    Grid3d gr;

    StandingWave() :gr(nx, ny, nz, a, b, a, b, a, a + d) {
        dt = d / (constants::c*sqrt(2) * 8);
        SetEB();
    }

    virtual void SetEB() {
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            for (int j = 0; j < gr.gnyRealNodes(); j++)
                for (int k = 0; k < gr.gnzRealNodes(); k++) {
                    gr(i, j, k).E = vec3<double>(0, 1, 0)*funcE(i*d + a, 0);
                    gr(i, j, k).B = vec3<double>(0, 0, 1)*funcB(i*d + a, 0);
                }

        FourierTransformation(gr, RtoC);
    }

    virtual double funcB(double x, double t) {
        return sin(2 * constants::pi / (b - a)*(-constants::c*t - x))+
            sin(2 * constants::pi / (b - a)*(+constants::c*t - x));
    }

    virtual double funcE(double x, double t) {
        return sin(2 * constants::pi / (b - a)*(constants::c*t + x))+
            sin(2 * constants::pi / (b - a)*(-constants::c*t + x));
    }
};


