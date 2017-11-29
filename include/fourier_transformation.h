#pragma once
class Grid3d;
#define RtoC (-1)
#define CtoR (+1)

typedef const int field;
static const field E = 0, B = 3;

void FourierTransformation(Grid3d& gr, field _field, int dir);