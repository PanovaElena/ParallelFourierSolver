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

struct ParametersForSphericalWave : public ParametersForMyTest {

    // физические параметры
    double T;   //период источника
    double omega;    // частота источника
    double omegaEnv;    //частота огибающей
    double Tx;    // период по координате 
    double Ty;

    ParametersForSphericalWave() {
        n.x = 128; n.y = n.x; n.z = 1;
        guard = vec3<int>(32);
        d = vec3<double>(constants::c);
        a = vec3<double>(0); b = a + n*d;
        dt = 0.1;
        nSeqSteps = 300;
        nParSteps = 100;
        nDomainSteps = (int) (0.4*guard.x*d.x / constants::c / dt);
        T = 16;
        omega = 2 * constants::pi / T;
        omegaEnv = omega;
        Tx = constants::c * 8; 
        Ty = Tx;
        fileWriter.Initialize("./", E, z, Section(Section::XOY, Section::center));
    }

    void print() {
        ParametersForMyTest::print();
        std::cout <<
            "omega = " << omega << "\n" <<
            "omega of envelope = " << omegaEnv << "\n" <<
            "time of sourse = " << T << "\n" <<
            "width of sourse = " << Tx << "\n" <<
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
        gr = Grid3d(parameters.n, parameters.a, parameters.a+parameters.n.x*parameters.d);
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
        int nIterJ = int(parameters.T / parameters.dt);
        if (iter > nIterJ) return 0;

        double t0 = 0.5*parameters.dt;
        if (parameters.fieldSolver.to_string() == "PSATD")
            t0 = 0;

        double x = GetX(i), y = GetY(j), t = iter*parameters.dt + t0;
        if (abs(i - parameters.n.x / 2) > parameters.Tx / parameters.d.x / 4 ||
            abs(j - parameters.n.y / 2) > parameters.Ty / parameters.d.y / 4)
            return 0;
        return pow(cos(2 * constants::pi*x / parameters.Tx), 2) *
            pow(cos(2 * constants::pi*y / parameters.Ty), 2) *
            sin(parameters.omega*t)*
            pow(sin(parameters.omegaEnv*t), 2);
    }

    void SetJ(int iter) {
        double J0;
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            for (int j = 0; j < gr.gnyRealNodes(); j++) {
                J0 = GetJ(i, j, iter);
                gr.J.Write(i, j, gr.gnzRealCells() / 2, vec3<double>(0, 0, J0));
            }
    }

    virtual void SetEB() {
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            for (int j = 0; j < gr.gnyRealNodes(); j++)
                for (int k = 0; k < gr.gnzRealNodes(); k++) {
                    gr.E.Write(i, j, k, vec3<double>(0, 0, 0));
                    gr.B.Write(i, j, k, vec3<double>(0, 0, 0));
                }

        SetJ(0);
    }

};

