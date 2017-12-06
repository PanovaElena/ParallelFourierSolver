#pragma once
class MyComplex;
using namespace std;

typedef const int field;

static const field Ex = 0, Ey = 1, Ez = 2;
static const field Bx = 3, By = 4, Bz = 5;

class OperationWithArrays {
public:
	static int GetIndex(int i, int j, int k, int Nx, int Ny, int Nz);
	static void WriteFromGridToArr(Grid3d& gr, vector<double>& arr, field _field);
	static void WriteFromGridToArr(Grid3d& gr, vector<MyComplex>& arr, field _field);
	static void WriteFromArrToGrid(Grid3d& gr, vector<MyComplex>& arr, field _field);
	static void WriteFromArrToGrid(Grid3d& gr, vector<double>& arr, field _field);
private:
	static void WriteFromGridToArrE(Grid3d& gr, vector<double>& arr, int coord);
	static void WriteFromGridToArrB(Grid3d& gr, vector<double>& arr, int coord);
	static void WriteFromArrToGridE(Grid3d& gr, vector<double>& arr, int coord);
	static void WriteFromArrToGridB(Grid3d& gr, vector<double>& arr, int coord);
	static void WriteFromGridToArrE(Grid3d& gr, vector<MyComplex>& arr, int coord);
	static void WriteFromGridToArrB(Grid3d& gr, vector<MyComplex>& arr, int coord);
	static void WriteFromArrToGridE(Grid3d& gr, vector<MyComplex>& arr, int coord);
	static void WriteFromArrToGridB(Grid3d& gr, vector<MyComplex>& arr, int coord);
};