#include "my_complex.h"
#include "grid3d.h"
#include <vector>
#include "operations_with_arrays.h"
using namespace std;

vec3<double> Node::* OperationWithArrays::DetPDouble(Field field) {
    vec3<double>(Node::*p);
    switch (field) {
    case Field::E:
        p = (vec3<double>(Node::*))&Node::E;
        break;
    case Field::B:
        p = (vec3<double>(Node::*))&Node::B;
        break;
    case Field::J:
        p = (vec3<double>(Node::*))&Node::J;
        break;
    }
    return p;
}

vec3<MyComplex> Node::* OperationWithArrays::DetPComplex(Field field) {
    vec3<MyComplex>(Node::*p);
    switch (field) {
    case Field::E:
        p = (vec3<MyComplex>(Node::*))&Node::EF;
        break;
    case Field::B:
        p = (vec3<MyComplex>(Node::*))&Node::BF;
        break;
    case Field::J:
        p = (vec3<MyComplex>(Node::*))&Node::JF;
        break;
    }
    return p;
}

void OperationWithArrays::CopyLastNodesFromFirst(Grid3d & gr)
{
    for (int i = 0; i < gr.gnxRealNodes(); i++)
        for (int k = 0; k < gr.gnzRealNodes(); k++)
		    gr(i, gr.gnyRealCells(), k) = gr(i, 0, k);
    for (int i = 0; i < gr.gnxRealNodes(); i++)
        for (int j = 0; j < gr.gnyRealNodes(); j++)
            gr(i, j, gr.gnzRealCells()) = gr(i, j, 0); 
    for (int j = 0; j < gr.gnyRealNodes(); j++)
        for (int k = 0; k < gr.gnzRealNodes(); k++)
            gr(gr.gnxRealCells(), j, k) = gr(0, j, k);
}

void OperationWithArrays::WriteDouble(Grid3d & gr, Field _field, Coordinate _coord, Direction direction, Array3d<double>& arr)
{
    vec3<double>(Node::*p);
    p = DetPDouble(_field);
    switch (direction) {
    case FromGridToArray:
        WriteFromGridToDoubleArr(gr, arr, (int)_coord, p);
        break;
    case FromArrayToGrid:
        WriteFromDoubleArrToGrid(gr, arr, (int)_coord, p);
        break;
    }
}

void OperationWithArrays::WriteComplex(Grid3d & gr, Field _field, Coordinate _coord, Direction direction, Array3d<MyComplex>& arr)
{
    vec3<MyComplex>(Node::*p);
    p = DetPComplex(_field);
    switch (direction) {
    case FromGridToArray:
        WriteFromGridToComplexArr(gr, arr, (int)_coord, p);
        break;
    case FromArrayToGrid:
        WriteFromComplexArrToGrid(gr, arr, (int)_coord, p);
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
    CopyLastNodesFromFirst(gr);
}

inline void OperationWithArrays::WriteFromComplexArrToGrid(Grid3d & gr, Array3d<MyComplex>& arr, int coord, vec3<MyComplex>(Node::* p))
{
	for (int i = 0; i < arr.gnx(); i++)
		for (int j = 0; j < arr.gny(); j++) {
			for (int k = 0; k < arr.gnz(); k++)
				(gr(i, j, k).*p)[coord] = arr(i, j, k);
			//FFTW
			//for (int k = gr.gnzRealCells() - 1; k > gr.gnzRealCells() / 2; k--)
				//(gr(i, j, k).*p)[coord] = arr(i, j, gr.gnzRealCells() - k).Conjugate();
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
