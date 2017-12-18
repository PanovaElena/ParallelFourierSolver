#include "fftw3.h"
#include "grid3d.h"
#include <vector>
#include "operations_with_arrays.h"
#include "my_complex.h"
#include "fourier_transformation.h"
using namespace std;

int GetSize(int Nx, int Ny, int Nz) {
	return (Nx*Ny*Nz);
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
		OperationWithArrays<double>::Write(gr, _field, FromGridToArray, Double, arrD);
		break;
	case  CtoR:
		OperationWithArrays<MyComplex>::Write(gr, _field, FromGridToArray, Complex, arrC);
		break;
	}

	UseFFTW(arrD, arrC, gr.gnxNodes(), gr.gnyNodes(), gr.gnzNodes(), dir);

	switch (dir) {
	case RtoC:
		OperationWithArrays<MyComplex>::Write(gr, _field, FromArrayToGrid, Complex, arrC);
		break;
	case  CtoR:
		OperationWithArrays<double>::Write(gr, _field, FromArrayToGrid, Double, arrD);
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
