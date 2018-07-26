#include "fftw3.h"
#include "grid3d.h"
#include <vector>
#include "operations_with_arrays.h"
#include "my_complex.h"
#include "fourier_transformation.h"
#include "physical_constants.h"
#include "array3d.h"
using namespace std;

double OmegaX(int i, const Grid3d& gr) {
	return (2 * constants::pi*((i <= gr.gnxRealCells() / 2) ? i : i - gr.gnxRealCells())) / (gr.gbx() - gr.gax());
}
double OmegaY(int j, const Grid3d& gr) {
	return (2 * constants::pi*((j <= gr.gnyRealCells() / 2) ? j : j - gr.gnyRealCells())) / (gr.gby() - gr.gay());
}
double OmegaZ(int k, const Grid3d& gr) {
	return (2 * constants::pi*((k <= gr.gnzRealCells() / 2) ? k : k - gr.gnzRealCells())) / (gr.gbz() - gr.gaz());
}

void UseFFTW(Array3d<double>& arr1, Array3d<MyComplex>& arr2, int Nx, int Ny, int Nz, int dir) {
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

void FourierTransformation(Grid3d & gr, Field _field, Coordinate _coord, int dir)
{
	Array3d<double> arrD(gr.gnxRealCells(), gr.gnyRealCells(), gr.gnzRealCells());
	Array3d<MyComplex> arrC(gr.gnxComplexCells(), gr.gnyComplexCells(), gr.gnzComplexCells());

    switch (dir) {
    case RtoC:
        OperationWithArrays::WriteDouble(gr, _field, _coord, FromGridToArray, arrD);
        break;
    case  CtoR:
        OperationWithArrays::WriteComplex(gr, _field, _coord, FromGridToArray, arrC);
        break;
    }
	UseFFTW(arrD, arrC, gr.gnxRealCells(), gr.gnyRealCells(), gr.gnzRealCells(), dir);

    switch (dir) {
    case RtoC:
        OperationWithArrays::WriteComplex(gr, _field, _coord, FromArrayToGrid, arrC);
        break;
    case  CtoR:
        OperationWithArrays::WriteDouble(gr, _field, _coord, FromArrayToGrid, arrD);
        break;
    }
}

void FourierTransformation(Grid3d & gr, int dir)
{
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
