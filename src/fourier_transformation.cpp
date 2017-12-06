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

void FourierTransformation(Grid3d & gr, field _field, int dir)
{
	vector<double> arrD(GetSize(gr.gnx()+1, gr.gny()+1, gr.gnz()+1));
	vector<MyComplex> arrC(GetSize(gr.gnx()+1, gr.gny()+1, gr.gnz()+1));

	switch (dir) {
	case RtoC:
		OperationWithArrays::WriteFromGridToArr(gr, arrD, _field);
		break;
	case  CtoR:
		OperationWithArrays::WriteFromGridToArr(gr, arrC, _field);
		break;
	}

	UseFFTW(arrD, arrC, gr.gnx(), gr.gny(), gr.gnz(), dir);

	switch (dir) {
	case RtoC:
		OperationWithArrays::WriteFromArrToGrid(gr, arrC, _field);
		break;
	case  CtoR:
		OperationWithArrays::WriteFromArrToGrid(gr, arrD, _field);
		break;
	}
}
