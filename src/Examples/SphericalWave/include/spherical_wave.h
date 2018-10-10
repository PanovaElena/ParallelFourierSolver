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
#include "physical_constants.h"
#include "field_solver.h"
#include "parameters_for_test.h"

struct ParametersForSphericalWave : public ParametersForTest {
    //метод
    FieldSolverType fieldSolver = FieldSolverPSATD;

    // сетка
    int nx = 150, ny = nx, nz = 1;
    int guard = 32;

    double d = constants::c;    // шаг сетки
    double a = 0, b = nx*d;    // координаты сетки

    // маска
    Mask mask = maskSineSquare;
    int maskWidth = 8;

    // физические параметры
    int wt = nx / 16;    // ширина синусоиды в ячейках
    double Tt = d*wt / constants::c;    // период по времени
    int wx = 12;
    double Tx = d*wx;    // период по координате 
    double Ty = Tx;

    // параметры счета
    int nConsSteps = 300;
    int nParSteps = 100;

    int nIterBetweenDumps = 100;

    double dt;

    ParametersForSphericalWave() : dt(0.1) {
    }

    int getNSteps() {
        return nConsSteps + nParSteps;
    }

    void print() {
        std::cout <<
            "dt = " << dt << "\n" <<
            "nx = " << nx << "\n" <<
            "ny = " << ny << "\n" <<
            "nz = " << nz << "\n" <<
            "guard = " << guard << "\n" <<
            "num of steps = " << getNSteps() << "\n" <<
            "num of consistent steps = " << nConsSteps << "\n" <<
            "num of parallel steps = " << nParSteps << "\n" <<
            std::endl;
    }
};

class SphericalWave {
public:

    // вывод
    std::string dir = std::string(ROOT_DIR) + "/files/spherical_wave/";
    Section section;
    FileWriter fileWriter;

    ParametersForSphericalWave parameters;
   
    Grid3d gr;

    SphericalWave() : parameters(), section(Section::XOY, Section::center) {
        fileWriter.Initialize(dir, E, z, section);
        Initialize();
    }

    void SetParamsForTest(ParametersForSphericalWave p) {
        parameters = p;
        Initialize();
    }

    void Initialize() {
        gr = Grid3d(parameters.nx, parameters.ny, parameters.nz, parameters.a, parameters.b, parameters.a, parameters.b, parameters.a, parameters.a + parameters.d);
        SetEB();
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
        return sin(2 * constants::pi / parameters.Tx*t);
    }

    double GetJ(int i, int j, int iter) {
        double x = GetX(i), y = GetY(j), t = iter*parameters.dt;
        if (iter > parameters.wt) return 0;
        if (abs(i - parameters.nx / 2) > parameters.wx / 4 || abs(j - parameters.ny / 2) >  parameters.wx / 4) return 0;
        return cos(2 * constants::pi*x / parameters.Tx)*cos(2 * constants::pi*x / parameters.Tx) *
            cos(2 * constants::pi*y / parameters.Ty)*cos(2 * constants::pi*y / parameters.Ty) *
            sin(2 * constants::pi*t / parameters.Tt);
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

    virtual void SetEB() {
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            for (int j = 0; j < gr.gnyRealNodes(); j++)
                for (int k = 0; k < gr.gnzRealNodes(); k++) {
                    gr(i, j, k).E = vec3<double>(0, 0, 0);
                    gr(i, j, k).B = vec3<double>(0, 0, 0);
                }

        FourierTransformation(gr, RtoC);
    }

};
