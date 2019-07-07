#include "gtest.h"
#include <cmath>
#include <string>
#include "physical_constants.h"
#include "grid3d.h"
#include "fourier_transform.h"
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

    int nIterBetweenDumps = params.getNSteps() / 10;

    TestSphericalWave() :
        fileWriterE(seqDir + dirE, E, z, params.fileWriter.getSection()),
        fileWriterB(seqDir + dirB, B, z, params.fileWriter.getSection()),
        fileWriterJ(seqDir + dirJ, J, z, params.fileWriter.getSection()) {}

    void MyTestBody() {

        write(fileWriterE, 0);
        write(fileWriterB, 0);

        transformGridIfNecessary(params.fieldSolver, gr, RtoC);

        for (int iter = 1; iter <= params.getNSteps(); iter++) {

            transformGridIfNecessary(params.fieldSolver, gr, CtoR);
            SetJ(iter, gr);
            transformGridIfNecessary(params.fieldSolver, gr, RtoC);

            params.fieldSolver(gr, params.dt);

            if (iter%nIterBetweenDumps == 0) {
                transformGridIfNecessary(params.fieldSolver, gr, CtoR);

                write(fileWriterE, iter);
                write(fileWriterB, iter);
                write(fileWriterJ, iter);

                transformGridIfNecessary(params.fieldSolver, gr, RtoC);
            }

        }

        write(fileWriterE, params.getNSteps(), "last_iter.csv");
        write(fileWriterB, params.getNSteps(), "last_iter.csv");
        write(fileWriterJ, params.getNSteps(), "last_iter.csv");
    }

    void write(FileWriter& fileWriter, int iter, std::string name = "") {
        if (name == "")
            name = "iter_" + std::to_string(iter) + "_coord_" +
            std::to_string(fileWriter.getCoord()) + ".csv";
        fileWriter.write(gr, name);
    }

    void PlotJ() {
        int iter = 2;
        SetJ(iter, gr);
        write(fileWriterJ, iter);
    }

};

TEST_F(TestSphericalWave, DISABLED_writing) {
    MyTestBody();
    PlotJ();
}