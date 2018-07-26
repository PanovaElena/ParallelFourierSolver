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
        fileWriterE(consDir + dirE, E, z, Section(Section::XOY, Section::center)),
        fileWriterB(consDir + dirB, B, z, Section(Section::XOY, Section::center)),
        fileWriterJ(consDir + dirJ, J, z, Section(Section::XOY, Section::center)) {}

    void MyTestBody() {
        FourierTransformation(gr, RtoC);

        WriteFile(fileWriterE, 0);
        WriteFile(fileWriterB, 0);

        for (int iter = 1; iter <= maxIt; iter++) {
            SetJ(iter);

            FieldSolver(gr, dt);

            if (iter%itTransform == 0) {
                FourierTransformation(gr, CtoR);

                WriteFile(fileWriterE, iter);
                WriteFile(fileWriterB, iter);
                WriteFile(fileWriterJ, iter);
            }
        }
    }

    void WriteFile(FileWriter& fileWriter, int iter) {
        std::string name="iter_" + std::to_string(iter) + ".csv";
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