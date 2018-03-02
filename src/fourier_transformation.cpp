#include "fftw3.h"
#include "grid3d.h"
#include <vector>
#include "operations_with_arrays.h"
#include "my_complex.h"
#include "fourier_transformation.h"
#include "constants.h"
using namespace std;

int GetSize(int Nx, int Ny, int Nz) {
	return (Nx*Ny*Nz);
}


double OmegaX(int i, const Grid3d& gr) {
	return (2 * constants::pi*((i <= gr.gnxCells() / 2) ? i : i - gr.gnxCells())) / (gr.gbx() - gr.gax());
}
double OmegaY(int j, const Grid3d& gr) {
	return (2 * constants::pi*((j <= gr.gnyCells() / 2) ? j : j - gr.gnyCells())) / (gr.gby() - gr.gay());
}
double OmegaZ(int k, const Grid3d& gr) {
	return (2 * constants::pi*((k <= gr.gnzCells() / 2) ? k : k - gr.gnzCells())) / (gr.gbz() - gr.gaz());
}

void UseFFTW(vector<double>& arr1, vector<MyComplex>& arr2, int Nx, int Ny, int Nz, int dir) {
	fftw_plan plan = 0;
	switch (dir) {
	case RtoC:
		plan = fftw_plan_dft_r2c_3d(Nx, Ny, Nz, &(arr1[0]), (fftw_complex*)&(arr2[0]), FFTW_ESTIMATE);
		fftw_execute(plan);
		break;
	case CtoR:
		plan = fftw_plan_dft_c2r_3d(Nx, Ny, Nz, (fftw_complex*)&(arr2[0]), &(arr1[0]), FFTW_ESTIMATE);
		fftw_execute(plan);
		for (int i = 0; i < GetSize(Nx, Ny, Nz); i++)
			arr1[i] /= Nx*Ny*Nz;
		break;
	}
	fftw_destroy_plan(plan);
}

void FourierTransformation(Grid3d & gr, Field _field, int dir)
{
	vector<double> arrD(GetSize(gr.gnxNodes(), gr.gnyNodes(), gr.gnzNodes()));
	vector<MyComplex> arrC(GetSize(gr.gnxNodes(), gr.gnyNodes(), gr.gnzNodes()));

	switch (dir) {
	case RtoC:
		OperationWithArrays::WriteDouble(gr, _field, FromGridToArray, arrD);
		break;
	case  CtoR:
		OperationWithArrays::WriteComplex(gr, _field, FromGridToArray, arrC);
		break;
	}

	UseFFTW(arrD, arrC, gr.gnxNodes(), gr.gnyNodes(), gr.gnzNodes(), dir);

	switch (dir) {
	case RtoC:
		OperationWithArrays::WriteComplex(gr, _field, FromArrayToGrid, arrC);
		break;
	case  CtoR:
		OperationWithArrays::WriteDouble(gr, _field, FromArrayToGrid, arrD);
		break;
	}
}

void FourierTransformation(Grid3d & gr, int dir)
{
	FourierTransformation(gr, Ex, dir);
	FourierTransformation(gr, Ey, dir);
	FourierTransformation(gr, Ez, dir);
	FourierTransformation(gr, Bx, dir);
	FourierTransformation(gr, By, dir);
	FourierTransformation(gr, Bz, dir);
}
