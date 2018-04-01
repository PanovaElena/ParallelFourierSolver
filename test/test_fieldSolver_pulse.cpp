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

double w = 10;
const double a = -w * constants::pi, b = w*constants::pi;
int n = 32;
const int maxIt = 32 + 1;
const int itTransform = 1;
const double d = (b - a) / n;
const double E0 = 1;

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

    TestPulse() :gr(n, n, n, a, b, a, b, a, b) {
        dt = d / constants::c;
        gr(n / 2, n / 2, n / 2).E = vec3<double>(E0, E0, E0);
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
                file <<(gr(i, j, gr.gnzRealCells() / 2).*GetField(field)).getNorm() << ";";
            file << std::endl;
        }

        file.close();
    }


};

TEST_F(TestPulse, test_pulse_write_file) {
    MyTestBody();
}