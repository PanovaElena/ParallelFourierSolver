#include "my_complex.h"
#include "grid3d.h"
#include <vector>
#include "operations_with_arrays.h"
using namespace std;

int OperationWithArrays::GetIndex(int i, int j, int k, int Nx, int Ny, int Nz) {
	return (i*Ny*Nz + j*Nz + k);
}

vec3<int> OperationWithArrays::GetVecIndex(int index, int nx, int ny, int nz)
{
	int i, j, k;
	k = index % nz;
	int index1 = (index - k) / nz;
	j = index1 % ny;
	i = (index1 - j) / ny;
	return vec3<int>(i, j, k);
}

vec3<double> node::* OperationWithArrays::DetPDouble(Field field) {
	vec3<double>(node::*p);
	switch (field / 3) {
	case 0:
		p = (vec3<double>(node::*))&node::E;
		break;
	case 1:
		p = (vec3<double>(node::*))&node::B;
		break;
	}
	return p;
}

vec3<MyComplex> node::* OperationWithArrays::DetPComplex(Field field) {
	vec3<MyComplex>(node::*p);
	switch (field / 3) {
	case 0:
		p = (vec3<MyComplex>(node::*))&node::EF;
		break;
	case 1:
		p = (vec3<MyComplex>(node::*))&node::BF;
		break;
	}
	return p;
}

void OperationWithArrays::WriteDouble(Grid3d & gr, Field _field, Dir direction, std::vector<double>& arr)
{
	vec3<double>(node::*p);
	p = DetPDouble(_field);
	switch (direction) {
	case FromGridToArray:
		WriteFromGridToDoubleArr(gr, arr, _field % 3, p);
		break;
	case FromArrayToGrid:
		WriteFromDoubleArrToGrid(gr, arr, _field % 3, p);
		break;
	}
}

void OperationWithArrays::WriteComplex(Grid3d & gr, Field _field, Dir direction, std::vector<MyComplex>& arr)
{
	vec3<MyComplex>(node::*p);
	p = DetPComplex(_field);
	switch (direction) {
	case FromGridToArray:
		WriteFromGridToComplexArr(gr, arr, _field % 3, p);
		break;
	case FromArrayToGrid:
		WriteFromComplexArrToGrid(gr, arr, _field % 3, p);
		break;
	}
}

void OperationWithArrays::WriteFromGridToDoubleArr(Grid3d& gr, std::vector<double>& arr, int coord, vec3<double>(node::*p)) {
	for (int i = 0; i <= gr.gnxCells(); i++)
		for (int j = 0; j <= gr.gnyCells(); j++)
			for (int k = 0; k <= gr.gnzCells(); k++)
			{
				arr[GetIndex(i, j, k, gr.gnxNodes(), gr.gnyNodes(), gr.gnzNodes())] = (gr(i, j, k).*p)[coord];
			}
}

void OperationWithArrays::WriteFromDoubleArrToGrid(Grid3d& gr, std::vector<double>& arr, int coord, vec3<double>(node::*p)) {
	for (int i = 0; i <= gr.gnxCells(); i++)
		for (int j = 0; j <= gr.gnyCells(); j++)
			for (int k = 0; k <= gr.gnzCells(); k++)
			{
				(gr(i, j, k).*p)[coord] = arr[GetIndex(i, j, k, gr.gnxNodes(), gr.gnyNodes(), gr.gnzNodes())];
			}
}

inline void OperationWithArrays::WriteFromComplexArrToGrid(Grid3d & gr, std::vector<MyComplex>& arr, int coord, vec3<MyComplex>(node::* p))
{
	for (int index = 0; index < arr.size() / 2 + 1; index++) {
		vec3<int> vecIndex = GetVecIndex(index, gr.gnxNodes(), gr.gnyNodes(), gr.gnzNodes() / 2 + 1);
		(gr(vecIndex).*p)[coord] = arr[index];
		if (vecIndex.z() != 0 && !(gr.gnzNodes()%2==0 && vecIndex.z()==gr.gnzNodes()/2)) {
			vecIndex[2] = gr.gnzNodes() - vecIndex.z();
			(gr(vecIndex).*p)[coord] = arr[index].Conjugate();
		}
	}
}

inline void OperationWithArrays::WriteFromGridToComplexArr(Grid3d & gr, std::vector<MyComplex>& arr, int coord, vec3<MyComplex>(node::* p))
{
	for (int i = 0; i<gr.gnxNodes(); i++)
		for (int j = 0; j<gr.gnyNodes(); j++)
			for (int k = 0; k < gr.gnzNodes() / 2 + 1; k++) {
				arr[GetIndex(i, j, k, gr.gnxNodes(), gr.gnyNodes(), gr.gnzNodes() / 2 + 1)] = (gr(i, j, k).*p)[coord];
			}
}
