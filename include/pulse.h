#pragma once
#include "constants.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include "grid3d.h"
#include "fourier_transformation.h"
#include "class_member_ptr.h"

class Pulse {
public:
    const int n = 64;
    const int k = 2;
    const double a = 0, b = n*k * constants::c;
    const double d = k * constants::c;

    const int w = 4; //ширина синусоиды в €чейках
    const double Tx = d*w / constants::c;
    const double Tt = d*w;
    const double dt = d / constants::c;

    Grid3d gr;

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
        return sin(2 * constants::pi / Tx*(t - r / constants::c));
    }

    Pulse() :gr(n, n, n, a, b, a, b, a, b) {
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            for (int j = 0; j < gr.gnyRealNodes(); j++)
                for (int k = 0; k < gr.gnzRealNodes(); k++) {
                    gr(i, j, k).E = vec3<double>(0, 0, 0);
                    gr(i, j, k).B = vec3<double>(0, 0, 0);
                }
        FourierTransformation(gr, RtoC);
    }

    void SetJ(int iter) {
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            for (int j = 0; j < gr.gnyRealNodes(); j++)
                for (int k = 0; k < gr.gnzRealNodes(); k++)
                    gr(i, j, k).J = vec3<double>(0, 0, 0);
        if (iter < w) {
            double J0 = f(0, iter*dt);
            gr(n / 2, n / 2, n / 2).J = vec3<double>(0, 0, J0);
        }
        FourierTransformation(gr, Jx, RtoC);
        FourierTransformation(gr, Jy, RtoC);
        FourierTransformation(gr, Jz, RtoC);
    }

};
