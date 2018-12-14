#pragma once
#include "physical_constants.h"
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

struct ParametersForSphericalWave : public ParametersForTest {
    //метод
    FieldSolver fieldSolver = PSATD;

    // сетка
    int nx = 128, ny = nx, nz = 1;
    int guard = 32;

    double d = constants::c;    // шаг сетки
    double a = 0, b = nx*d;    // координаты сетки

    // маска
    Mask mask = MaskSimple;
    int maskWidth = 8;

    // фильтр
    Filter filter;

    // физические параметры
    double T = 16;   //период источника
    double omega = 2*constants::pi/T;    // частота источника
    double omegaEnv = omega;    //частота огибающей
    double Tx = constants::c*8;    // период по координате 
    double Ty = Tx;

    // параметры счета
    int nConsSteps = 300;
    int nParSteps = 100;

    int nIterBetweenDumps = 100;

    double dt;

    ParametersForSphericalWave() : dt(0.1) {
        nIterBetweenDumps = nParSteps;
    }

    int getNSteps() {
        return nConsSteps + nParSteps;
    }

    void print() {
        std::cout <<
            "field solver: " << fieldSolver.to_string() << "\n" <<
            "dt = " << dt << "\n" <<
            "omega = " << omega << "\n" <<
            "omegaEnv = " << omegaEnv << "\n" <<
            "T = " << T << "\n" <<
            "TCoord = " << Tx << "\n" <<
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
        gr = Grid3d(parameters.nx, parameters.ny, parameters.nz, 
            parameters.a, parameters.a+parameters.nx*parameters.d,
            parameters.a, parameters.a + parameters.ny*parameters.d, 
            parameters.a, parameters.a + parameters.nz*parameters.d);
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
        int nIterJ = int(parameters.T / parameters.dt)+1;
        if (iter > nIterJ) return 0;
        double x = GetX(i), y = GetY(j), t = iter*parameters.dt;
        if (abs(i - parameters.nx / 2) > parameters.Tx / parameters.d / 4 ||
            abs(j - parameters.ny / 2) > parameters.Ty / parameters.d / 4)
            return 0;
        return pow(cos(2 * constants::pi*x / parameters.Tx), 2) *
            pow(cos(2 * constants::pi*y / parameters.Ty), 2) *
            sin(parameters.omega*t)*
            pow(sin(parameters.omegaEnv*t), 2);
    }

    void SetJ(int iter) {
        if (iter != 0) TransformGridIfNecessary(parameters.fieldSolver, gr, CtoR);
        double J0;
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            for (int j = 0; j < gr.gnyRealNodes(); j++) {
                J0 = GetJ(i, j, iter);
                gr.J.Write(i, j, gr.gnzRealCells() / 2, vec3<double>(0, 0, J0));
            }
        if (iter != 0) TransformGridIfNecessary(parameters.fieldSolver, gr, RtoC);
    }

    virtual void SetEB() {
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            for (int j = 0; j < gr.gnyRealNodes(); j++)
                for (int k = 0; k < gr.gnzRealNodes(); k++) {
                    gr.E.Write(i, j, k, vec3<double>(0, 0, 0));
                    gr.B.Write(i, j, k, vec3<double>(0, 0, 0));
                }

        //TransformGridIfNecessary(parameters.fieldSolver, gr, RtoC);
        SetJ(0);
    }

};

