#include "gtest.h"
#include "constants.h"
#include <cmath>
#include <string>
#include "grid3d.h"
#include "fourier_transformation.h"
#include "class_member_ptr.h"
#include "spherical_wave.h"
#include "write_file.h"
#include "field_solver.h"

class TestSphericalWave :public testing::Test, public SphericalWave {
public:

    std::string consDir = "consistent_results/";
    std::string dirE = dir.substr(3, dir.length()-3)+consDir+"E/";
    std::string dirB = dir.substr(3, dir.length() - 3) + consDir + "B/";
    std::string dirJ = dir.substr(3, dir.length() - 3) + consDir + "J/";

    void MyTestBody() {
        FourierTransformation(gr, RtoC);

        WriteFile(E, 0, dirE);
        WriteFile(B, 0, dirB);

        for (int iter = 1; iter <= maxIt; iter++) {
            SetJ(iter);

            FieldSolver(gr, dt);

            if (iter%itTransform == 0) {
                FourierTransformation(gr, CtoR);

                WriteFile(E, iter, dirE);
                WriteFile(B, iter, dirB);
                WriteFile(J, iter, dirJ);
            }
        }
    }

    void WriteFile(Field field, int iter, std::string name) {
        writeFile(field, z, gr, name + "iter_" + std::to_string(iter) + ".csv");
    }

    void PlotJ() {
        int iter = 2;
        SetJ(iter);
        WriteFileField2d(J, z, gr, dirJ+"FuncJ.csv");
    }

};

TEST_F(TestSphericalWave, test_SphericalWave_write_file) {
    MyTestBody();
    PlotJ();
}