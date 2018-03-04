#include "my_complex.h"
#include "grid3d.h"
#include <vector>
#include "operations_with_arrays.h"
using namespace std;

vec3<double> Node::* OperationWithArrays::DetPDouble(Field field) {
	vec3<double>(Node::*p);
	switch (field / 3) {
	case 0:
		p = (vec3<double>(Node::*))&Node::E;
		break;
	case 1:
		p = (vec3<double>(Node::*))&Node::B;
		break;
	}
	return p;
}

vec3<MyComplex> Node::* OperationWithArrays::DetPComplex(Field field) {
	vec3<MyComplex>(Node::*p);
	switch (field / 3) {
	case 0:
		p = (vec3<MyComplex>(Node::*))&Node::EF;
		break;
	case 1:
		p = (vec3<MyComplex>(Node::*))&Node::BF;
		break;
	}
	return p;
}

void OperationWithArrays::WriteDouble(Grid3d & gr, Field _field, Dir direction, Array3d<double>& arr)
{
	vec3<double>(Node::*p);
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

void OperationWithArrays::WriteComplex(Grid3d & gr, Field _field, Dir direction, Array3d<MyComplex>& arr)
{
	vec3<MyComplex>(Node::*p);
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

void OperationWithArrays::WriteFromGridToDoubleArr(Grid3d& gr, Array3d<double>& arr, int coord, vec3<double>(Node::*p)) {
	for (int i = 0; i < arr.gnx(); i++)
		for (int j = 0; j < arr.gny(); j++)
			for (int k = 0; k < arr.gnz(); k++)
			{
				arr(i, j, k) = (gr(i, j, k).*p)[coord];
			}
}

void OperationWithArrays::WriteFromDoubleArrToGrid(Grid3d& gr, Array3d<double>& arr, int coord, vec3<double>(Node::*p)) {
	for (int i = 0; i < arr.gnx(); i++)
		for (int j = 0; j < arr.gny(); j++)
			for (int k = 0; k < arr.gnz(); k++)
			{
				(gr(i, j, k).*p)[coord] = arr(i, j, k);
			}
}

inline void OperationWithArrays::WriteFromComplexArrToGrid(Grid3d & gr, Array3d<MyComplex>& arr, int coord, vec3<MyComplex>(Node::* p))
{
	for (int i = 0; i < arr.gnx(); i++)
		for (int j = 0; j < arr.gny(); j++) {
			for (int k = 0; k < arr.gnz(); k++)
				(gr(i, j, k).*p)[coord] = arr(i, j, k);
			//for (int k = gr.gnzCells() - 1; k > gr.gnzCells() / 2; k--)
				//(gr(i, j, k).*p)[coord] = arr(i, j, gr.gnzCells() - k).Conjugate();
		}
}

inline void OperationWithArrays::WriteFromGridToComplexArr(Grid3d & gr, Array3d<MyComplex>& arr, int coord, vec3<MyComplex>(Node::* p))
{
	for (int i = 0; i < arr.gnx(); i++)
		for (int j = 0; j < arr.gny(); j++) 
			for (int k = 0; k < arr.gnz(); k++) {
				arr(i, j, k) = (gr(i, j, k).*p)[coord];
			}
}
