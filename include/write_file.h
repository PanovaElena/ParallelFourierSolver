#pragma once
class Grid3d;
#include "simple_types.h"
#include <string>

typedef void(*TypeWriteFileField)(Field field, Coords coord, Grid3d& gr, std::string nameFile);
typedef void(*TypeWriteFileFieldAbs)(Field field, Grid3d& gr, std::string nameFile);

// поле при z=n/2
void WriteFileFieldAbs2d(Field field, Grid3d& gr, std::string nameFile);
void WriteFileField2d(Field field, Coords coord, Grid3d& gr, std::string nameFile);

// поле при y=n/2 и z=n/2
void WriteFileFieldAbs1d(Field field, Grid3d& gr, std::string nameFile);
void WriteFileField1d(Field field, Coords coord, Grid3d& gr, std::string nameFile);