#pragma once
#include "grid3d.h"
#include "physical_constants.h"
#include "simple_types_and_constants.h"
#include "fourier_transformation.h"
#include "field_solver.h"
#include "class_member_ptr.h"
#include "file_writer.h"
#include "masks.h"

class RunningWaveWithHighFrequency {
public:

    // �����
    std::string dir = "../../../files/running_wave_with_high_frequency/";
    FileWriter fileWriter;

    // �����
    int nx = 128, ny = nx, nz = 1;
    int guard = 64;

    MaskSineSquare mask;

    double a = 0, b = nx;    // ���������� �����
    double d = 1;    // ��� �����

    // ���������� ���������
    double Tx = 8 * d; //������ 

    // ��������� �����
    int NStartSteps = 600;
    int NNextSteps = 200;

    int maxIt = NStartSteps + NNextSteps;
    int itTransform = 100;

    double dt;


    Grid3d gr;

    RunningWaveWithHighFrequency() :gr(nx, ny, nz, a, b, a, b, a, a + d),
        fileWriter(dir, E, y, Section(Section::XOY, Section::center)), mask(8) {
        dt = d / (constants::c*sqrt(2) * 8);
        SetEB();
    }

    virtual double funcB(double x, double t) {
        return sin(2 * constants::pi / Tx*(-constants::c*t - x));
    }

    virtual double funcE(double x, double t) {
        return sin(2 * constants::pi / Tx*(constants::c*t + x));
    }

    virtual void SetEB() {
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            for (int j = 0; j < gr.gnyRealNodes(); j++)
                for (int k = 0; k < gr.gnzRealNodes(); k++) {
                    gr(i, j, k).E = vec3<double>(0, 1, 0)*funcE(i*d + a, 0);
                    gr(i, j, k).B = vec3<double>(0, 0, 1)*funcB(i*d + a, 0);
                }

        FourierTransformation(gr, RtoC);
    }
};


