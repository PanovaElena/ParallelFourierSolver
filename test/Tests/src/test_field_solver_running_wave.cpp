#include "gtest.h"
#include <cmath>
#include <string>
#include "running_wave.h"
#include "field_solver.h"
#include "file_writer.h"

class TestRunningWave :public testing::Test, public RunningWave  {
public:

    std::string consDir = dir + "consistent_results/";
    std::string dirE = "E/";
    std::string dirB = "B/";

    FileWriter fileWriterE;
    FileWriter fileWriterB;

    TestRunningWave() :
        fileWriterE(consDir + dirE, E, y, section),
        fileWriterB(consDir + dirB, B, z, section) {}

    void WriteFile(FileWriter& fileWriter, int iter, std::string name="") {
        if (name == "")
            name = "iter_" + std::to_string(iter) + "_coord_" + 
            std::to_string(fileWriter.getCoord()) + ".csv";
        fileWriter.WriteFile(gr, name);
    }

    void MyTestBodyCheckOnNull() {

        for (int j = 0; j <= parameters.nConsSteps+ parameters.nParSteps; j++) {
            parameters.fieldSolver(gr, parameters.dt);

            if (j%parameters.nIterBetweenDumps == 0) {
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

        WriteFile(fileWriterE, 0);
        WriteFile(fileWriterB, 0);

        for (int j = 1; j <= parameters.getNSteps(); j++) {
            parameters.fieldSolver(gr, parameters.dt);

            if (j%parameters.nIterBetweenDumps == 0) {
                FourierTransformation(gr, CtoR);

                WriteFile(fileWriterE, j);
                WriteFile(fileWriterB, j);
            }
        }
        

        WriteFile(fileWriterE, parameters.getNSteps(), "last_iter.csv");
        WriteFile(fileWriterB, parameters.getNSteps(), "last_iter.csv");

    }

    ~TestRunningWave() {}
};


TEST_F(TestRunningWave, other_components_is_null_Ey_Bz) {
    MyTestBodyCheckOnNull();
}

TEST_F(TestRunningWave, res_is_sinus_Ey_Bz) {
    MyTestBodyWriteFile();
}
