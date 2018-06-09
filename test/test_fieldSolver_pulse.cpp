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
#include "pulse.h"
#include "write_file.h"

static std::ofstream file;
static const std::string strE = "../../files/field_solver_test_pulse_E/";
static const std::string strB = "../../files/field_solver_test_pulse_B/";
static const std::string strJ = "../../files/field_solver_test_pulse_J/";

const int maxIt = 40;
const int itTransform = 40;

class TestPulse :public testing::Test, public Pulse {
public:
    void MyTestBody() {
        FourierTransformation(gr, RtoC);

        WriteFile(E, 0, strE);
        WriteFile(B, 0, strB);

        for (int iter = 1; iter <= maxIt; iter++) {
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
        WriteFileField(field, z, gr, name + "iter_" + std::to_string(iter) + ".csv");
    }

    void PlotJ() {
        int iter = 2;
        SetJ(iter);
        WriteFileField(J, z, gr, "../../files/field_solver_test_pulse_J/FuncJ_iter2.csv");
    }

};

TEST_F(TestPulse, test_pulse_write_file) {
    MyTestBody();
    PlotJ();
}