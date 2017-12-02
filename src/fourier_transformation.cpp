#include "fftw3.h"
#include "grid3d.h"
#include <vector>
#include "operations_with_arrays.h"
#include "my_complex.h"
#include "fourier_transformation.h"
using namespace std;

int GetIndex(int i, int j, int k, int Nx, int Ny, int Nz) {
	return (i*Ny*Nz + j*Nz + k);
}
int GetSize(int Nx, int Ny, int Nz) {
	return (Nx*Ny*Nz);
}

void UseFFTW(vector<double>& arr1, vector<MyComplex>& arr2, int Nx, int Ny, int Nz, int dir) {
	fftw_plan plan = 0;
	switch (dir) {
	case RtoC:
		plan = fftw_plan_dft_r2c_3d(Nx, Ny, Nz, &(arr1[0]), (fftw_complex*)&(arr2[0]), FFTW_ESTIMATE);
		fftw_execute_dft_r2c(plan, &(arr1[0]), (fftw_complex*)&(arr2[0]));
		break;
	case CtoR:
		plan = fftw_plan_dft_c2r_3d(Nx, Ny, Nz, (fftw_complex*)&(arr2[0]), &(arr1[0]), FFTW_ESTIMATE);
		fftw_execute_dft_c2r(plan, (fftw_complex*)&(arr2[0]), &(arr1[0]));
		break;
	}
	fftw_destroy_plan(plan);
}

void FourierTransformation(Grid3d & gr, field _field, int dir)
{
	vector<double> arrD(GetSize(gr.gnx(), gr.gny(), gr.gnz()));
	vector<MyComplex> arrC(GetSize(gr.gnx(), gr.gny(), gr.gnz()));

	switch (dir) {
	case RtoC:
		WriteFromGridToArr(gr, arrD, _field);
		break;
	case  CtoR:
		WriteFromGridToArr(gr, arrC, _field);
		break;
	}

	UseFFTW(arrD, arrC, gr.gnx(), gr.gny(), gr.gnz(), dir);

	switch (dir) {
	case RtoC:
		WriteFromArrToGrid(gr, arrD, _field);
		break;
	case  CtoR:
		WriteFromArrToGrid(gr, arrC, _field);
		break;
	}
}
