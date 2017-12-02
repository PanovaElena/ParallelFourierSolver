#include "my_complex.h"
#include "grid3d.h"
#include <vector>
#include "operations_with_arrays.h"
using namespace std;

int GetIndex(int i, int j, int k, int Nx, int Ny, int Nz) {
	return (i*Ny*Nz + j*Nz + k);
}

void WriteFromGridToArr(Grid3d& gr, vector<double>& arr, field _field) {
	if (_field / 3 == 0) WriteFromGridToArrE(gr, arr, _field % 3);
	else WriteFromGridToArrB(gr, arr, _field % 3);
}
void WriteFromGridToArrE(Grid3d& gr, vector<double>& arr, int coord) {
		for (int i = 0; i < gr.gnx(); i++)
			for (int j = 0; j < gr.gny(); j++)
				for (int k = 0; k < gr.gnz(); k++)
				{
					arr[GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())] = gr(i, j, k).E[coord];
				}
}
void WriteFromGridToArrB(Grid3d& gr, vector<double>& arr, int coord) {
	for (int i = 0; i < gr.gnx(); i++)
		for (int j = 0; j < gr.gny(); j++)
			for (int k = 0; k < gr.gnz(); k++)
			{
				arr[GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())] = gr(i, j, k).B[coord];
			}
}

void WriteFromGridToArr(Grid3d& gr, vector<MyComplex>& arr, field _field) {
	if (_field / 3 == 0) WriteFromGridToArrE(gr, arr, _field % 3);
	else WriteFromGridToArrB(gr, arr, _field % 3);
}
void WriteFromGridToArrE(Grid3d& gr, vector<MyComplex>& arr, int coord) {
	for (int i = 0; i < gr.gnx(); i++)
		for (int j = 0; j < gr.gny(); j++)
			for (int k = 0; k < gr.gnz(); k++)
			{
				arr[GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())] = gr(i, j, k).EF[coord];
			}
}
void WriteFromGridToArrB(Grid3d& gr, vector<MyComplex>& arr, int coord) {
	for (int i = 0; i < gr.gnx(); i++)
		for (int j = 0; j < gr.gny(); j++)
			for (int k = 0; k < gr.gnz(); k++)
			{
				arr[GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())] = gr(i, j, k).BF[coord];
			}
}

void WriteFromArrToGrid(Grid3d& gr, vector<MyComplex>& arr, field _field) {
	if (_field / 3 == 0) WriteFromArrToGridE(gr, arr, _field % 3);
	else WriteFromArrToGridB(gr, arr, _field % 3);
}
void WriteFromArrToGridE(Grid3d& gr, vector<MyComplex>& arr, int coord) {
	for (int i = 0; i < gr.gnx(); i++)
		for (int j = 0; j < gr.gny(); j++)
			for (int k = 0; k < gr.gnz(); k++)
			{
				gr(i, j, k).EF[coord] = arr[GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())];
			}
}
void WriteFromArrToGridB(Grid3d& gr, vector<MyComplex>& arr, int coord) {
	for (int i = 0; i < gr.gnx(); i++)
		for (int j = 0; j < gr.gny(); j++)
			for (int k = 0; k < gr.gnz(); k++)
			{
				gr(i, j, k).BF[coord] = arr[GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())];
			}
}

void WriteFromArrToGrid(Grid3d& gr, vector<double>& arr, field _field) {
	if (_field / 3 == 0) WriteFromArrToGridE(gr, arr, _field % 3);
	else WriteFromArrToGridB(gr, arr, _field % 3);
}
void WriteFromArrToGridE(Grid3d& gr, vector<double>& arr, int coord) {
	for (int i = 0; i < gr.gnx(); i++)
		for (int j = 0; j < gr.gny(); j++)
			for (int k = 0; k < gr.gnz(); k++)
			{
				gr(i, j, k).E[coord] = arr[GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())];
			}
}
void WriteFromArrToGridB(Grid3d& gr, vector<double>& arr, int coord) {
	for (int i = 0; i < gr.gnx(); i++)
		for (int j = 0; j < gr.gny(); j++)
			for (int k = 0; k < gr.gnz(); k++)
			{
				gr(i, j, k).B[coord] = arr[GetIndex(i, j, k, gr.gnx(), gr.gny(), gr.gnz())];
			}
}



