#pragma once
#include "grid3d.h"
#include "my_complex.h"
#include "simple_types.h"

template <class T>
class OperationWithArrays {
public:
	static int GetIndex(int i, int j, int k, int Nx, int Ny, int Nz);
	static void Write(Grid3d& gr, Field _field, Dir direction, Type type, std::vector<T>& arr);
private:
	static void WriteFromGridToArr(Grid3d& gr, std::vector<T>& arr, int coord, vec3<T>(node::*p));
	static void WriteFromArrToGrid(Grid3d& gr, std::vector<T>& arr, int coord, vec3<T>(node::*p));

	static vec3<T> node::* DetP(Type type, Field field);
};


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
			p = (vec3<T>(node::*))&node::E;
			break;
		case Complex:
			p = (vec3<T>(node::*))&node::EF;
			break;
		}
		break;
	case 1:
		switch (type) {
		case Double:
			p = (vec3<T>(node::*))&node::B;
			break;
		case Complex:
			p = (vec3<T>(node::*))&node::BF;
			break;
		}
		break;

	}
	return p;
}

template<class T>
void OperationWithArrays<T>::Write(Grid3d& gr, Field _field, Dir direction, Type type, std::vector<T>& arr) {
	vec3<T>(node::*p);
	p = DetP(type, _field);

	switch (direction) {
	case FromGridToArray:
		WriteFromGridToArr(gr, arr, _field % 3, p);
		break;
	case FromArrayToGrid:
		WriteFromArrToGrid(gr, arr, _field % 3, p);
		break;
	}
}

template<class T>
void OperationWithArrays<T>::WriteFromGridToArr(Grid3d& gr, std::vector<T>& arr, int coord, vec3<T>(node::*p)) {
	for (int i = 0; i <= gr.gnxCells(); i++)
		for (int j = 0; j <= gr.gnyCells(); j++)
			for (int k = 0; k <= gr.gnzCells(); k++)
			{
				arr[GetIndex(i, j, k, gr.gnxNodes(), gr.gnyNodes(), gr.gnzNodes())] = (gr(i, j, k).*p)[coord] ;
			}
}

template<class T>
void OperationWithArrays<T>::WriteFromArrToGrid(Grid3d& gr, std::vector<T>& arr, int coord, vec3<T>(node::*p)) {
	for (int i = 0; i <= gr.gnxCells(); i++)
		for (int j = 0; j <= gr.gnyCells(); j++)
			for (int k = 0; k <= gr.gnzCells(); k++)
			{
				(gr(i, j, k).*p)[coord] = arr[GetIndex(i, j, k, gr.gnxNodes(), gr.gnyNodes(), gr.gnzNodes())];
			}
}
