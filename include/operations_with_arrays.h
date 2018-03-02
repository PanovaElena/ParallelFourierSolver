#pragma once
#include "grid3d.h"
#include "my_complex.h"
#include "simple_types.h"
#include <vector>

class OperationWithArrays {
public:
	
	static void WriteDouble(Grid3d& gr, Field _field, Dir direction, std::vector<double>& arr);
	static void WriteComplex(Grid3d& gr, Field _field, Dir direction, std::vector<MyComplex>& arr);

	static void WriteFromGridToDoubleArr(Grid3d& gr, std::vector<double>& arr, int coord, vec3<double>(node::*p));
	static void WriteFromDoubleArrToGrid(Grid3d& gr, std::vector<double>& arr, int coord, vec3<double>(node::*p));

	static void WriteFromComplexArrToGrid(Grid3d& gr, std::vector<MyComplex>& arr, int coord, vec3<MyComplex>(node::*p));
	static void WriteFromGridToComplexArr(Grid3d& gr, std::vector<MyComplex>& arr, int coord, vec3<MyComplex>(node::*p));

private:
	static vec3<double> node::* DetPDouble(Field field);
	static vec3<MyComplex> node::* DetPComplex(Field field);

	static vec3<int> GetVecIndex(int index, int nx, int ny, int nz);
	static int GetIndex(int i, int j, int k, int Nx, int Ny, int Nz);
};