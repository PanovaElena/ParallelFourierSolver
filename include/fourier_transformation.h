#pragma once
#include "operations_with_arrays.h"
class Grid3d;
#define RtoC (-1)
#define CtoR (+1)

void FourierTransformation(Grid3d& gr, Field _field, Coordinate _coord, int dir);
void FourierTransformation(Grid3d& gr, int dir);

double OmegaX(int i, const Grid3d& gr);
double OmegaY(int j, const Grid3d& gr);
double OmegaZ(int k, const Grid3d& gr);
