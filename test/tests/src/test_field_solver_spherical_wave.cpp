#include "gtest.h"
#include "physical_constants.h"
#include <cmath>
#include <string>
#include "grid3d.h"
#include "fourier_transformation.h"
#include "class_member_ptr.h"
#include "spherical_wave.h"
#include "file_writer.h"
#include "field_solver.h"

class TestSphericalWave :public testing::Test, public SphericalWave {
public:

    std::string seqDir = std::string(ROOT_DIR) + "/files/spherical_wave/test/";
    std::string dirE = "E/";
    std::string dirB = "B/";
    std::string dirJ = "J/";

    FileWriter fileWriterE;
    FileWriter fileWriterB;
    FileWriter fileWriterJ;

    int nIterBetweenDumps = parameters.getNSteps() / 10;

    TestSphericalWave() : 
        fileWriterE(seqDir + dirE, E, z, parameters.fileWriter.getSection()),
        fileWriterB(seqDir + dirB, B, z, parameters.fileWriter.getSection()),
        fileWriterJ(seqDir + dirJ, J, z, parameters.fileWriter.getSection()) {}

    void MyTestBody() {

        WriteFile(fileWriterE, 0);
        WriteFile(fileWriterB, 0);

        TransformGridIfNecessary(parameters.fieldSolver, gr, RtoC);

        for (int iter = 1; iter <= parameters.getNSteps(); iter++) {

            TransformGridIfNecessary(parameters.fieldSolver, gr, CtoR);
            SetJ(iter, gr);
            TransformGridIfNecessary(parameters.fieldSolver, gr, RtoC);

            parameters.fieldSolver(gr, parameters.dt);

            if (iter%nIterBetweenDumps == 0) {
                TransformGridIfNecessary(parameters.fieldSolver, gr, CtoR);

                WriteFile(fileWriterE, iter);
                WriteFile(fileWriterB, iter);
                WriteFile(fileWriterJ, iter);

                TransformGridIfNecessary(parameters.fieldSolver, gr, RtoC);
            }

        }

        WriteFile(fileWriterE, parameters.getNSteps(), "last_iter.csv");
        WriteFile(fileWriterB, parameters.getNSteps(), "last_iter.csv");
        WriteFile(fileWriterJ, parameters.getNSteps(), "last_iter.csv");
    }

    void WriteFile(FileWriter& fileWriter, int iter, std::string name = "") {
        if (name == "")
            name = "iter_" + std::to_string(iter) + "_coord_" +
            std::to_string(fileWriter.getCoord()) + ".csv";
        fileWriter.WriteFile(gr, name);
    }

    void PlotJ() {
        int iter = 2;
        SetJ(iter, gr);
        WriteFile(fileWriterJ, iter);
    }

};

TEST_F(TestSphericalWave, writing) {
    MyTestBody();
    PlotJ();
}