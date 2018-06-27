#pragma once
#include "constants.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include "grid3d.h"
#include "fourier_transformation.h"
#include "class_member_ptr.h"
#include "write_file.h"

class SphericalWave {
public:

    std::string dir = "../../../files/spherical_wave/";

    TypeWriteFileField writeFile = WriteFileField2d;

    int NStartSteps = 300;
    int NNextSteps = 100;

    int maxIt = NStartSteps + NNextSteps;
    int itTransform = 100;

    int nx = 128, ny = nx, nz = 1;
    int guard = 16;

    double a = 0, b = nx* constants::c;    // координаты сетки
    double d = constants::c;    // шаг сетки

    int wt = nx / 16;    // ширина синусоиды в ячейках
    double Tt = d*wt / constants::c;    // период по T
    double dt = d / constants::c / 10;
    int wx = 12; 
    double Tx = d*wx;    // период по координате 
    double Ty = Tx;

    Grid3d gr;

    SphericalWave() :gr(nx, ny, nz, a, b, a, b, 0, d) {
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            for (int j = 0; j < gr.gnyRealNodes(); j++)
                for (int k = 0; k < gr.gnzRealNodes(); k++) {
                    gr(i, j, k).E = vec3<double>(0, 0, 0);
                    gr(i, j, k).B = vec3<double>(0, 0, 0);
                }
        FourierTransformation(gr, RtoC);
    }

    double sign(double x) {
        return x >= 0 ? 1 : -1;
    }

    double GetX(int i) {
        return (i - gr.gnxRealCells() / 2)*gr.gdx();
    }

    double GetY(int j) {
        return (j - gr.gnyRealCells() / 2)*gr.gdy();
    }

    double GetZ(int k) {
        return (k - gr.gnzRealCells() / 2)*gr.gdz();
    }

    double f(double r, double t) {
        return sin(2 * constants::pi / Tx*t);
    }

    double GetJ(int i, int j, int iter) {
        double x = GetX(i), y = GetY(j), t = iter*dt;
        if (iter > wt) return 0;
        if (abs(i - nx / 2) > wx / 4 || abs(j - ny / 2) > wx / 4) return 0;
        return cos(2 * constants::pi*x / Tx)*cos(2 * constants::pi*x / Tx) *
            cos(2 * constants::pi*y / Ty)*cos(2 * constants::pi*y / Ty) *
            sin(2 * constants::pi*t / Tt);
    }

    void SetJ(int iter) {
        double J0;
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            for (int j = 0; j < gr.gnyRealNodes(); j++) {
                J0 = GetJ(i, j, iter);
                gr(i, j, gr.gnzRealCells() / 2).J = vec3<double>(0, 0, J0);
            }
        FourierTransformation(gr, Jx, RtoC);
        FourierTransformation(gr, Jy, RtoC);
        FourierTransformation(gr, Jz, RtoC);
    }

};
