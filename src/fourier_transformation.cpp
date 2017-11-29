#include "fftw3.h"
#include "grid3d.h"
#include <vector>
#include <string>
#include <complex>
#include "fourier_transformation.h"
using namespace std;

const int NField = 3;
static const field Ex = 0, Ey = 1, Ez = 2;
static const field Bx = 3, By = 4, Bz = 5;

fftw_complex complex_i = { 0,1 };

int GetIndex(int i, int j, int k, int Nx, int Ny, int Nz) {
	return (i*Ny*Nz + j*Nz + k);
}

//двумерный массив [3][Nx*Ny*Nz]: 1 координата -  поле x,y,z ; 2 координата - элемент 3-хмерного массива
template <class T>
vector<vector<T> > CreateArr(int Nx, int Ny, int Nz) {
	vector<vector<T> > arr(NField);
	for (int i = 0; i < NField; i++)
		arr[i] = vector<T>(Nx*Ny*Nz);
	return arr;
}

void WriteFromGridToArrE(Grid3d& gr, vector<vector<double> >& arr) {
	for (int i = 0; i < gr.gnx(); i++)
		for (int j = 0; j < gr.gny(); j++)
			for (int k = 0; k < gr.gnz(); k++)
			{
				arr[Ex][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())] = gr(i, j, k).E.x;
				arr[Ey][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())] = gr(i, j, k).E.y;
				arr[Ez][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())] = gr(i, j, k).E.z;
			}
}
void WriteFromGridToArrE(Grid3d& gr, vector<vector<complex<double> > >& arr) {
	for (int i = 0; i < gr.gnx(); i++)
		for (int j = 0; j < gr.gny(); j++)
			for (int k = 0; k < gr.gnz(); k++)
			{
				arr[Ex][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())] = gr(i, j, k).EF.x;
				arr[Ey][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())] = gr(i, j, k).EF.y;
				arr[Ez][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())] = gr(i, j, k).EF.z;
			}
}
void WriteFromGridToArrB(Grid3d& gr, vector<vector<double> >& arr) {
	for (int i = 0; i < gr.gnx(); i++)
		for (int j = 0; j < gr.gny(); j++)
			for (int k = 0; k < gr.gnz(); k++)
			{
				arr[Bx][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())] = gr(i, j, k).B.x;
				arr[By][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())] = gr(i, j, k).B.y;
				arr[Bz][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())] = gr(i, j, k).B.z;
			}
}
void WriteFromGridToArrB(Grid3d& gr, vector<vector<complex<double> > >& arr) {
	for (int i = 0; i < gr.gnx(); i++)
		for (int j = 0; j < gr.gny(); j++)
			for (int k = 0; k < gr.gnz(); k++)
			{
				arr[Bx][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())] = gr(i, j, k).BF.x;
				arr[By][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())] = gr(i, j, k).BF.y;
				arr[Bz][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())] = gr(i, j, k).BF.z;
			}
}
void WriteFromGridToArr(Grid3d& gr, vector<vector<double> >& arr, field _field) {
	switch (_field) {
	case E:
		WriteFromGridToArrE(gr, arr);
		break;
	case B:
		WriteFromGridToArrB(gr, arr);
		break;
	}
}
void WriteFromGridToArr(Grid3d& gr, vector<vector<complex<double> > >& arr, field _field) {
	switch (_field) {
	case E:
		WriteFromGridToArrE(gr, arr);
		break;
	case B:
		WriteFromGridToArrB(gr, arr);
		break;
	}
}

void WriteFromArrToGridE(Grid3d& gr, vector<vector<complex<double> > >& arr) {
	for (int i = 0; i < gr.gnx(); i++)
		for (int j = 0; j < gr.gny(); j++)
			for (int k = 0; k < gr.gnz(); k++)
			{
				gr(i, j, k).EF.x = arr[Ex][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())];
				gr(i, j, k).EF.y = arr[Ey][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())];
				gr(i, j, k).EF.z = arr[Ez][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())];
			}
}
void WriteFromArrToGridE(Grid3d& gr, vector<vector<double> >& arr) {
	for (int i = 0; i < gr.gnx(); i++)
		for (int j = 0; j < gr.gny(); j++)
			for (int k = 0; k < gr.gnz(); k++)
			{
				gr(i, j, k).E.x = arr[Ex][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())];
				gr(i, j, k).E.y = arr[Ey][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())];
				gr(i, j, k).E.z = arr[Ez][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())];
			}
}
void WriteFromArrToGridB(Grid3d& gr, vector<vector<complex<double> > >& arr) {
	for (int i = 0; i < gr.gnx(); i++)
		for (int j = 0; j < gr.gny(); j++)
			for (int k = 0; k < gr.gnz(); k++)
			{
				gr(i, j, k).BF.x = arr[Bx][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())];
				gr(i, j, k).BF.y = arr[By][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())];
				gr(i, j, k).BF.z = arr[Bz][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())];
			}
}
void WriteFromArrToGridB(Grid3d& gr, vector<vector<double> >& arr) {
	for (int i = 0; i < gr.gnx(); i++)
		for (int j = 0; j < gr.gny(); j++)
			for (int k = 0; k < gr.gnz(); k++)
			{
				gr(i, j, k).B.x = arr[Bx][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())];
				gr(i, j, k).B.y = arr[By][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())];
				gr(i, j, k).B.z = arr[Bz][GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())];
			}
}
void WriteFromArrToGrid(Grid3d& gr, vector<vector<complex<double> > >& arr, field _field) {
	switch (_field) {
	case E:
		WriteFromGridToArrE(gr, arr);
		break;
	case B:
		WriteFromGridToArrB(gr, arr);
		break;
	}
}
void WriteFromArrToGrid(Grid3d& gr, vector<vector<double> >& arr, field _field) {
	switch (_field) {
	case E:
		WriteFromGridToArrE(gr, arr);
		break;
	case B:
		WriteFromGridToArrB(gr, arr);
		break;
	}
}

void UseFFTW(vector<vector<double> >& arr1, vector<vector<complex<double> > >& arr2, int Nx, int Ny, int Nz, int dir) {
	for (int i = 0; i < NField; i++) {
		fftw_plan plan = 0;
		switch (dir) {
		case RtoC:
			plan = fftw_plan_dft_r2c_3d(Nx, Ny, Nz, &(arr1[i][0]), (fftw_complex*) &(arr2[i][0]), FFTW_ESTIMATE);
			fftw_execute_dft_r2c(plan, &(arr1[i][0]), (fftw_complex*) &(arr2[i][0]));
			break;
		case CtoR:
			plan = fftw_plan_dft_c2r_3d(Nx, Ny, Nz, (fftw_complex*) &(arr2[i][0]), &(arr1[i][0]), FFTW_ESTIMATE);
			fftw_execute_dft_c2r(plan, (fftw_complex*) &(arr2[i][0]), &(arr1[i][0]));
			break;
		}
		fftw_destroy_plan(plan);
	}
}

void FourierTransformation(Grid3d & gr, field _field, int dir)
{
	vector<vector<double> > arrD = CreateArr<double>(gr.gnx(), gr.gny(), gr.gnz());
	vector<vector< complex<double> > > arrC = CreateArr<complex<double> >(gr.gnx(), gr.gny(), gr.gnz());

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
