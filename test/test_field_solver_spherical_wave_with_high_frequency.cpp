#include "gtest.h"
#include <cmath>
#include <string>
#include "running_wave_with_high_frequency.h"
#include "field_solver.h"
#include "write_file.h"

class TestRunningWaveWithHighFrequency :public testing::Test, public RunningWaveWithHighFrequency {
public:

    std::string consDir = "consistent_results/";
    std::string dirE = dir.substr(3, dir.length() - 3) + consDir + "E/";
    std::string dirB = dir.substr(3, dir.length() - 3) + consDir + "B/";

    void WriteFile(Field field, Coords coord, int iter, std::string name) {
        writeFile(field, coord, gr,
            name + "iter_" + std::to_string(iter) + "_coord_" + std::to_string(coord) + ".csv");
    }

    void MyTestBodyCheckOnNull() {

        for (int j = 0; j <= maxIt; j++) {
            FieldSolver(gr, dt);

            if (j%itTransform == 0) {
                FourierTransformation(gr, CtoR);
            }


            for (int i = 0; i < gr.gnxRealCells(); i++) {
                ASSERT_NEAR(0, (gr(i, 0, 0).*GetField(B).*GetCoord(x))(), 1E-5);
                ASSERT_NEAR(0, (gr(i, 0, 0).*GetField(B).*GetCoord(y))(), 1E-5);
                ASSERT_NEAR(0, (gr(i, 0, 0).*GetField(E).*GetCoord(x))(), 1E-5);
                ASSERT_NEAR(0, (gr(i, 0, 0).*GetField(E).*GetCoord(z))(), 1E-5);
            }


            /*WriteFile(B, othCoordB1, mainC, j, dirB + "test_zeros_");
            WriteFile(B, othCoordB2, mainC, j, dirB + "test_zeros_");
            WriteFile(E, othCoordE1, mainC, j, dirE + "test_zeros_");
            WriteFile(E, othCoordE2, mainC, j, dirE + "test_zeros_");*/
        }
    }

    void MyTestBodyWriteFile() {

        for (int j = 0; j <= maxIt; j++) {
            FieldSolver(gr, dt);

            if (j%itTransform == 0) {
                FourierTransformation(gr, CtoR);

                WriteFile(E, y, j, dirE);
                WriteFile(B, z, j, dirB);
            }
        }

    }

    ~TestRunningWaveWithHighFrequency() {}
};


TEST_F(TestRunningWaveWithHighFrequency, other_components_is_null_Ey_Bz) {
    MyTestBodyCheckOnNull();
}

TEST_F(TestRunningWaveWithHighFrequency, res_is_sinus_Ey_Bz) {
    MyTestBodyWriteFile();
}
