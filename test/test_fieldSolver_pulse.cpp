#include "gtest.h"
#include "field_solver_for_testing.h"
#include "constants.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include "grid3d.h"
#include "fourier_transformation.h"
#include "class_member_ptr.h"

static std::ofstream file;
static const std::string strE = "../../files/field_solver_test_pulse_E/";
static const std::string strB = "../../files/field_solver_test_pulse_B/";
static const std::string strJ = "../../files/field_solver_test_pulse_J/";

const int n = 64;
const int k = 2;
const double a = 0, b = n*k * constants::c;
const double d = k * constants::c;

const int w = 5; //ширина синусоиды в €чейках
const double Tx = d*w / constants::c;
const double Tt = d*w;
const double dt = d / constants::c;

const int maxIt = 32;
const int itTransform = 1;


class TestPulse :public testing::Test {
public:
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
        return sin(constants::pi / Tx*(t - r / constants::c));
    }

    TestPulse() :gr(n, n, n, a, b, a, b, a, b) {
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            for (int j = 0; j < gr.gnyRealNodes(); j++)
                for (int k = 0; k < gr.gnzRealNodes(); k++) {
                    gr(i, j, k).E = vec3<double>(0, 0, 0);
                    gr(i, j, k).B = vec3<double>(0, 0, 0);
                }
    }

    void SetJ(int iter) {
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            for (int j = 0; j < gr.gnyRealNodes(); j++)
                for (int k = 0; k < gr.gnzRealNodes(); k++)
                    gr(i, j, k).J = vec3<double>(0, 0, 0);
        if (iter < 2*w) {
            double J0 = 4*constants::pi*f(0, iter*dt);
            gr(n / 2, n / 2, n / 2).J = vec3<double>(J0, J0, J0);
        }
        FourierTransformation(gr, Jx, RtoC);
        FourierTransformation(gr, Jy, RtoC);
        FourierTransformation(gr, Jz, RtoC);
    }

    void MyTestBody() {
        FourierTransformation(gr, RtoC);

        WriteFile(E, 0, strE);
        WriteFile(B, 0, strB);

        for (int iter = 1; iter < maxIt; iter++) {
            SetJ(iter);

            FieldSolver(gr, dt);

            if (iter%itTransform == 0) {
                FourierTransformation(gr, CtoR);

                WriteFile(E, iter, strE);
                WriteFile(B, iter, strB);
                WriteFile(J, iter, strJ);
            }
        }
    }

    void WriteFile(Field field, int iter, std::string name) {
        file.open(name + "iter_" + std::to_string(iter) + ".csv");//freopen?

        for (int i = 0; i < gr.gnxRealCells(); i++) {
                file <<(gr(i, gr.gnyRealCells() / 2, gr.gnzRealNodes() / 2).*GetField(field)).getNorm() << ";";
            file << std::endl;
        }

        file.close();
    }


};

TEST_F(TestPulse, test_pulse_write_file) {
    MyTestBody();
}