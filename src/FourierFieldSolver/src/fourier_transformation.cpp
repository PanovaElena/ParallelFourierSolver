#include <iostream>
#include "fftw3.h"
#include "grid3d.h"
#include "my_complex.h"
#include "fourier_transformation.h"
#include "array3d.h"
#include "class_member_ptr.h"

void UseFFTW(Array3d<double>& arr1, Array3d<MyComplex>& arr2, int Nx, int Ny, int Nz, Direction dir) {
    fftw_plan plan = 0;
    switch (dir) {
    case RtoC:
        plan = fftw_plan_dft_r2c_3d(Nx, Ny, Nz, &(arr1[0]), (fftw_complex*)&(arr2[0]), FFTW_ESTIMATE);
        fftw_execute(plan);
        break;
    case CtoR:
        plan = fftw_plan_dft_c2r_3d(Nx, Ny, Nz, (fftw_complex*)&(arr2[0]), &(arr1[0]), FFTW_ESTIMATE);
        fftw_execute(plan);
        for (int i = 0; i < Nx*Ny*Nz; i++)
            arr1[i] /= Nx*Ny*Nz;
        break;
    }
    fftw_destroy_plan(plan);
}

void FourierTransformation(Grid3d & gr, Field _field, Coordinate _coord, Direction dir)
{
    //if (gr.getLastFourierTransform(_field, _coord) == dir) {
    //    //std::cout << "Try to transform to the same direction: " << dir << std::endl;
    //    return;
    //}

    Array3d<double>& arrD = (gr.*GetField<double>(_field)).*GetFieldCoord<double>(_coord);
    Array3d<MyComplex>& arrC = (gr.*GetField<MyComplex>(_field)).*GetFieldCoord<MyComplex>(_coord);

    UseFFTW(arrD, arrC, gr.gnxRealCells(), gr.gnyRealCells(), gr.gnzRealCells(), dir);

    gr.setLastFourierTransform(_field, _coord, dir);
}

void FourierTransformation(Grid3d & gr, Direction dir)
{
    if (gr.getLastFourierTransform(E, x) == dir) {
        std::cout << "Try to transform to the same direction: " << dir << std::endl;
        return;
    }

    FourierTransformation(gr, E, x, dir);
    FourierTransformation(gr, E, y, dir);
    FourierTransformation(gr, E, z, dir);
    FourierTransformation(gr, B, x, dir);
    FourierTransformation(gr, B, y, dir);
    FourierTransformation(gr, B, z, dir);
    FourierTransformation(gr, J, x, dir);
    FourierTransformation(gr, J, y, dir);
    FourierTransformation(gr, J, z, dir);
}
