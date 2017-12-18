#pragma once
#include "operations_with_arrays.h"
class Grid3d;
#define RtoC (-1)
#define CtoR (+1)

void FourierTransformation(Grid3d& gr, Field _field, int dir);
void FourierTransformation(Grid3d& gr, int dir);