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

    std::string consDir = dir.substr(3, dir.length() - 3) + "consistent_results/";
    std::string dirE = "E/";
    std::string dirB = "B/";
    std::string dirJ = "J/";

    FileWriter fileWriterE;
    FileWriter fileWriterB;
    FileWriter fileWriterJ;

    TestSphericalWave() : 
        fileWriterE(consDir + dirE, E, z, section),
        fileWriterB(consDir + dirB, B, z, section),
        fileWriterJ(consDir + dirJ, J, z, section) {}

    void MyTestBody() {
        FourierTransformation(gr, RtoC);

        WriteFile(fileWriterE, 0);
        WriteFile(fileWriterB, 0);

        for (int iter = 1; iter <= maxIt; iter++) {
            SetJ(iter);

            fieldSolver(gr, dt);

            if (iter%itTransform == 0) {
                FourierTransformation(gr, CtoR);

                WriteFile(fileWriterE, iter);
                WriteFile(fileWriterB, iter);
                WriteFile(fileWriterJ, iter);
            }

        }

        WriteFile(fileWriterE, maxIt, "last_iter.csv");
        WriteFile(fileWriterB, maxIt, "last_iter.csv");
        WriteFile(fileWriterJ, maxIt, "last_iter.csv");
    }

    void WriteFile(FileWriter& fileWriter, int iter, std::string name = "") {
        if (name == "")
            name = "iter_" + std::to_string(iter) + "_coord_" +
            std::to_string(fileWriter.getCoord()) + ".csv";
        fileWriter.WriteFile(gr, name);
    }

    void PlotJ() {
        int iter = 2;
        SetJ(iter);
        WriteFile(fileWriterJ, iter);
    }

};

TEST_F(TestSphericalWave, test_SphericalWave_write_file) {
    MyTestBody();
    PlotJ();
}