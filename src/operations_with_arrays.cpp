#include "my_complex.h"
#include "grid3d.h"
#include <vector>
#include "operations_with_arrays.h"
using namespace std;

template <class T>
int OperationWithArrays<T>::GetIndex(int i, int j, int k, int Nx, int Ny, int Nz) {
	return (i*Ny*Nz + j*Nz + k);
}

template<class T>
vec3<T> node::* OperationWithArrays<T>::DetP(Type type, Field field) {
	vec3<T>(node::*p);
	switch (field / 3) {
	case 0:
		switch (type) {
		case Double:
			p = &node::E;
			break;
		case Complex:
			p = &node::EF;
			break;
		}
		break;
	case 1:
		switch (type) {
		case Double:
			p = &node::B;
			break;
		case Complex:
			p = &node::BF;
			break;
		}
		break;

	}
	return p;
}

template<class T> 
void OperationWithArrays<T>::Write(Grid3d& gr, Field _field, Dir direction, Type type, vector<T>& arr) {
	vec3<T>(node::*p);
	p = DetP(type, _field);

	switch (direction) {
	case FromGridToArray:
		WriteFromGridToArr(gr, arr, _field%3, p);
		break;
	case FromArrayToGrid:
		WriteFromArrToGrid(gr, arr, _field%3, p);
		break;
	}
}

template<class T>
void OperationWithArrays<T>::WriteFromGridToArr(Grid3d& gr, vector<T>& arr, int coord, vec3<double> (node::*p)) {
	for (int i = 0; i <= gr.gnxCells(); i++)
		for (int j = 0; j <= gr.gnyCells(); j++)
			for (int k = 0; k <= gr.gnzCells(); k++)
			{
				(gr(i, j, k).*p)[coord] = arr[GetIndex(i, j, k, gr.gnxCells(), gr.gnyCells(), gr.gnzCells())];
			}
}

template<class T>
void OperationWithArrays<T>::WriteFromArrToGrid(Grid3d& gr, vector<T>& arr, int coord, vec3<double>(node::*p)) {
	for (int i = 0; i <= gr.gnxCells(); i++)
		for (int j = 0; j <= gr.gnyCells(); j++)
			for (int k = 0; k <= gr.gnzCells(); k++)
			{
				(gr(i, j, k).*p)[coord] = arr[GetIndex(i, j, k, gr.gnxCells(), gr.gnyCells(), gr.gnzCells())];
			}
}
