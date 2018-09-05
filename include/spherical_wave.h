#pragma once
#include "physical_constants.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include "grid3d.h"
#include "fourier_transformation.h"
#include "class_member_ptr.h"
#include "file_writer.h"
#include "masks.h"
#include "field_solver.h"

class SphericalWave {
public:

    // вывод
    std::string dir = "../../../files/spherical_wave/";
    Section section;
    FileWriter fileWriter;

    //метод
    FieldSolverType fieldSolver = FieldSolverPSATD;

    // сетка
    int nx = 128, ny = nx, nz = 1;
    int guard = 32;

    Mask mask;
    int maskWidth = 8;

    double d = constants::c;    // шаг сетки
    double a = 0, b = nx*d;    // координаты сетки

    // физические параметры
    int wt = nx / 16;    // ширина синусоиды в ячейках
    double Tt = d*wt / constants::c;    // период по времени
    int wx = 12;
    double Tx = d*wx;    // период по координате 
    double Ty = Tx;

    // параметры счета
    int nStartSteps = 300;
    int nNextSteps = 100;

    int maxIt = nStartSteps + nNextSteps;
    int itTransform = 100;

    double dt = d / constants::c / 10;

   
    Grid3d gr;

    SphericalWave() :gr(nx, ny, nz, a, b, a, b, 0, d),
        section(Section::XOY, Section::center){
        mask = maskSineSquare;
        fileWriter.Initialize(dir, E, z, section);
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
        FourierTransformation(gr, J, x, RtoC);
        FourierTransformation(gr, J, y, RtoC);
        FourierTransformation(gr, J, z, RtoC);
    }

    void SetParamsForTest(FieldSolverType _fieldSolver, int n_cons_steps, int n_par_steps) {
        fieldSolver = _fieldSolver;
        nStartSteps = n_cons_steps;
        nNextSteps = n_par_steps;
        maxIt = nStartSteps + nNextSteps;
    }

};
