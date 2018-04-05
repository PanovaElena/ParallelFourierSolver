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

const int n = 64;
const double a = 0, b = 128;
const double d = (b - a) / n;

const int width = 5; //ширина кольца (число €чеек)
const double l = width*d;
const double t0 = 0;
const double N = 8;

const int maxIt = 32 + 1;
const int itTransform = 1;


class TestPulse :public testing::Test {
public:
    double dt;
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

    double mySin(double r, double t) {
        return sin(2 * constants::pi * N / (b - a)*(constants::c*t - r));
    }

    vec3<double> GetNormal(double x, double y, double z) {
        if (sqrt(x*x + y*y + z*z) == 0)
            return vec3<double>(0, 0, 0);
        return vec3<double>(1, 1, 1)*(1 / sqrt(x*x + y*y + z*z) / sqrt(3));
    }

    vec3<double> funcE(double x, double y, double z, double t) {
        return mySin(sqrt(x*x + y*y + z*z), t)*GetNormal(x, y, z);
    }

    vec3<double> funcB(double x, double y, double z, double t) {
        return -1 * funcE(x, y, z, t);
    }

    TestPulse() :gr(n, n, n, a, b, a, b, a, b) {
        dt = d / constants::c;

        for (int i = 0; i < gr.gnxRealNodes(); i++)
            for (int j = 0; j < gr.gnyRealNodes(); j++)
                for (int k = 0; k < gr.gnzRealNodes(); k++) {
                    gr(i, j, k).E = funcE(GetX(i), GetY(j), GetZ(k), t0);
                    gr(i, j, k).B = funcB(GetX(i), GetY(j), GetZ(k), t0);
                }


        FourierTransformation(gr, RtoC);
    }

    void MyTestBody() {

        WriteFile(E, 0, strE);
        WriteFile(B, 0, strB);

        for (int j = 1; j < maxIt; j++) {
            FieldSolver(gr, dt);

            if (j%itTransform == 0) {
                FourierTransformation(gr, CtoR);

                WriteFile(E, j, strE);
                WriteFile(B, j, strB);
            }
        }
    }

    void WriteFile(Field field, int iter, std::string name) {
        file.open(name + "iter_" + std::to_string(iter) + ".csv");//freopen?

        for (int i = 0; i < gr.gnxRealNodes(); i++) {
            for (int j = 0; j < gr.gnxRealNodes(); j++)
                file <<(gr(i, j, gr.gnzRealNodes() / 2).*GetField(field)).getNorm() << ";";
            file << std::endl;
        }

        file.close();
    }


};

TEST_F(TestPulse, test_pulse_write_file) {
    MyTestBody();
}