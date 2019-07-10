#include <omp.h>
#include "fftw3.h"
#include "fftw3-mpi.h"
#include "mpi_wrapper.h"
#include "grid3d.h"
#include "my_complex.h"
#include "fourier_transform.h"
#include "array3d.h"
#include "class_member_ptr.h"

void useFFTW(Array3d<double>& arr1, Array3d<MyComplex>& arr2, int Nx, int Ny, int Nz,
    Direction dir, bool useMpi=false) {
    fftw_plan plan = 0;
    fftw_plan_with_nthreads(omp_get_max_threads());
    switch (dir) {
    case RtoC:
        if (useMpi) {
            MPIWrapper::showMessage("doing direct mpi fft");
            plan = fftw_mpi_plan_dft_r2c_3d(Nx, Ny, Nz, &(arr1[0]), (fftw_complex*)&(arr2[0]),
                MPI_COMM_WORLD, FFTW_ESTIMATE);
        }
        else
            plan = fftw_plan_dft_r2c_3d(Nx, Ny, Nz, &(arr1[0]), (fftw_complex*)&(arr2[0]), FFTW_ESTIMATE);
        fftw_execute(plan);
        break;
    case CtoR:
        if (useMpi) {
            MPIWrapper::showMessage("doing inverse mpi fft");
            plan = fftw_mpi_plan_dft_c2r_3d(Nx, Ny, Nz, (fftw_complex*)&(arr2[0]), &(arr1[0]),
                MPI_COMM_WORLD, FFTW_ESTIMATE);
        }
        else
            plan = fftw_plan_dft_c2r_3d(Nx, Ny, Nz, (fftw_complex*)&(arr2[0]), &(arr1[0]), FFTW_ESTIMATE);
        fftw_execute(plan);
        for (int i = 0; i < Nx * Ny * Nz; i++)
            arr1[i] /= Nx * Ny * Nz;
        break;
    }
    fftw_destroy_plan(plan);
}

void fourierTransform(Grid3d & gr, Field _field, Coordinate _coord, Direction dir, bool useMpi) {
    Array3d<double>& arrD = (gr.*getMemberPtrField<double>(_field)).*getMemberPtrFieldCoord<double>(_coord);
    Array3d<MyComplex>& arrC = (gr.*getMemberPtrField<MyComplex>(_field)).*getMemberPtrFieldCoord<MyComplex>(_coord);

    useFFTW(arrD, arrC, gr.sizeReal().x, gr.sizeReal().y, gr.sizeReal().z, dir, useMpi);
}

void fourierTransform(Grid3d & gr, Direction dir, bool useMpi) {
    if (gr.getLastFourierTransformDirect() == dir) {
        std::cout << "Try to transform to the same direction: " << dir << std::endl;
        return;
    }

    fourierTransform(gr, E, x, dir, useMpi);
    fourierTransform(gr, E, y, dir, useMpi);
    fourierTransform(gr, E, z, dir, useMpi);
    fourierTransform(gr, B, x, dir, useMpi);
    fourierTransform(gr, B, y, dir, useMpi);
    fourierTransform(gr, B, z, dir, useMpi);
    fourierTransform(gr, J, x, dir, useMpi);
    fourierTransform(gr, J, y, dir, useMpi);
    fourierTransform(gr, J, z, dir, useMpi);

    gr.setLastFourierTransformDirect(dir);
}
