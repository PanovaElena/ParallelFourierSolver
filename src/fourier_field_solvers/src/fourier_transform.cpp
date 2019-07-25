#include <omp.h>
#include "mpi_wrapper.h"
#include "fftw3.h"
#include "grid3d.h"
#include "my_complex.h"
#include "fourier_transform.h"
#include "array3d.h"
#include "class_member_ptr.h"

void useFFTW(Array3d<double>& arr1, Array3d<MyComplex>& arr2, int Nx, int Ny, int Nz,
    Direction dir) {
    fftw_plan plan = 0;
    fftw_plan_with_nthreads(omp_get_max_threads());
    switch (dir) {
    case RtoC:
        plan = fftw_plan_dft_r2c_3d(Nx, Ny, Nz, &(arr1[0]), (fftw_complex*)&(arr2[0]), FFTW_ESTIMATE);
        fftw_execute(plan);
        break;
    case CtoR:
        plan = fftw_plan_dft_c2r_3d(Nx, Ny, Nz, (fftw_complex*)&(arr2[0]), &(arr1[0]), FFTW_ESTIMATE);
        fftw_execute(plan);
        for (int i = 0; i < Nx * Ny * Nz; i++)
            arr1[i] /= Nx * Ny * Nz;
        break;
    }
    fftw_destroy_plan(plan);
}

void fourierTransform(Grid3d& gr, Field _field, Coordinate _coord, Direction dir) {
    Array3d<double>& arrD = (gr.*getMemberPtrField<double>(_field)).*getMemberPtrFieldCoord<double>(_coord);
    Array3d<MyComplex>& arrC = (gr.*getMemberPtrField<MyComplex>(_field)).*getMemberPtrFieldCoord<MyComplex>(_coord);

    useFFTW(arrD, arrC, gr.sizeReal().x, gr.sizeReal().y, gr.sizeReal().z, dir);
}

void fourierTransform(Grid3d& gr, Direction dir) {
    if (gr.getLastFourierTransformDirect() == dir) {
        std::cout << "Try to transform to the same direction: " << dir << std::endl;
        return;
    }

    fourierTransform(gr, E, x, dir);
    fourierTransform(gr, E, y, dir);
    fourierTransform(gr, E, z, dir);
    fourierTransform(gr, B, x, dir);
    fourierTransform(gr, B, y, dir);
    fourierTransform(gr, B, z, dir);
    fourierTransform(gr, J, x, dir);
    fourierTransform(gr, J, y, dir);
    fourierTransform(gr, J, z, dir);

    gr.setLastFourierTransformDirect(dir);
}
