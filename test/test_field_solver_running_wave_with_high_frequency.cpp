#include "gtest.h"
#include <cmath>
#include <string>
#include "running_wave_with_high_frequency.h"
#include "field_solver.h"
#include "file_writer.h"

class TestRunningWaveWithHighFrequency :public testing::Test, public RunningWaveWithHighFrequency {
public:

    std::string consDir = dir.substr(3, dir.length() - 3) + "consistent_results/";
    std::string dirE = "E/";
    std::string dirB = "B/";

    FileWriter fileWriterE;
    FileWriter fileWriterB;

    TestRunningWaveWithHighFrequency() :
        fileWriterE(consDir + dirE, E, y, Section(Section::XOY, Section::center)),
        fileWriterB(consDir + dirB, B, z, Section(Section::XOY, Section::center)) {}

    void WriteFile(FileWriter& fileWriter, int iter) {
        std::string name = "iter_" + std::to_string(iter) + "_coord_" + 
            std::to_string(fileWriter.getCoord()) + ".csv";
        fileWriter.WriteFile(gr, name);
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
        }
    }

    void MyTestBodyWriteFile() {

        for (int j = 0; j <= maxIt; j++) {
            FieldSolver(gr, dt);

            if (j%itTransform == 0) {
                FourierTransformation(gr, CtoR);

                WriteFile(fileWriterE, j);
                WriteFile(fileWriterB, j);
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
