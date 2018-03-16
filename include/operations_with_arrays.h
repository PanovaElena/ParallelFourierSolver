#pragma once
#include "grid3d.h"
#include "my_complex.h"
#include "simple_types.h"
#include "array3d.h"

class OperationWithArrays {
public:
    
    static void WriteDouble(Grid3d& gr, Field _field, Dir direction, Array3d<double>& arr);
    static void WriteComplex(Grid3d& gr, Field _field, Dir direction, Array3d<MyComplex>& arr);

    static void WriteFromGridToDoubleArr(Grid3d& gr, Array3d<double>& arr, int coord, vec3<double>(Node::*p));
    static void WriteFromDoubleArrToGrid(Grid3d& gr, Array3d<double>& arr, int coord, vec3<double>(Node::*p));

    static void WriteFromComplexArrToGrid(Grid3d& gr, Array3d<MyComplex>& arr, int coord, vec3<MyComplex>(Node::*p));
    static void WriteFromGridToComplexArr(Grid3d& gr, Array3d<MyComplex>& arr, int coord, vec3<MyComplex>(Node::*p));

private:
	static vec3<double> Node::* DetPDouble(Field field);
	static vec3<MyComplex> Node::* DetPComplex(Field field);

	static void CopyLastNodesFromFirst(Grid3d& grid);
};