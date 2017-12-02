#pragma once
class MyComplex;
using namespace std;

typedef const int field;

static const field Ex = 0, Ey = 1, Ez = 2;
static const field Bx = 3, By = 4, Bz = 5;

int GetIndex(int i, int j, int k, int Nx, int Ny, int Nz);
void WriteFromGridToArr(Grid3d& gr, vector<double>& arr, field _field);
void WriteFromGridToArr(Grid3d& gr, vector<MyComplex>& arr, field _field);
void WriteFromArrToGrid(Grid3d& gr, vector<MyComplex>& arr, field _field);
void WriteFromArrToGrid(Grid3d& gr, vector<double>& arr, field _field);