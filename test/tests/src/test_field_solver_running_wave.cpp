#include "gtest.h"
#include <cmath>
#include <string>
#include "running_wave.h"
#include "field_solver.h"
#include "file_writer.h"

class TestRunningWave :public testing::Test, public RunningWave {
public:
    std::string seqDir = std::string(ROOT_DIR) + "/files/running_wave/test/";
    std::string dirE = "E/";
    std::string dirB = "B/";

    FileWriter fileWriterE;
    FileWriter fileWriterB;

    int nIterBetweenDumps = parameters.getNSteps() / 10;

    TestRunningWave() :
        fileWriterE(seqDir + dirE, E, y, parameters.fileWriter.getSection()),
        fileWriterB(seqDir + dirB, B, z, parameters.fileWriter.getSection()) {}

    void write(FileWriter& fileWriter, int iter, std::string name = "") {
        if (name == "")
            name = "iter_" + std::to_string(iter) + "_coord_" +
            std::to_string(fileWriter.getCoord()) + ".csv";
        fileWriter.write(gr, name);
    }

    void MyTestBodyWrite() {

        write(fileWriterE, 0);
        write(fileWriterB, 0);

        transformGridIfNecessary(parameters.fieldSolver, gr, RtoC);

        for (int j = 1; j <= parameters.getNSteps(); j++) {
            parameters.fieldSolver(gr, parameters.dt);

            if (j%nIterBetweenDumps == 0) {
                transformGridIfNecessary(parameters.fieldSolver, gr, CtoR);

                write(fileWriterE, j);
                write(fileWriterB, j);

                transformGridIfNecessary(parameters.fieldSolver, gr, RtoC);
            }
        }


        write(fileWriterE, parameters.getNSteps(), "last_iter.csv");
        write(fileWriterB, parameters.getNSteps(), "last_iter.csv");

    }

    ~TestRunningWave() {}
};

TEST_F(TestRunningWave, DISABLED_writing_Ey_Bz) {
    MyTestBodyWrite();
}
