#pragma once
class Grid3d;
#include "simple_types.h"
#include <string>

void WriteFileFieldAbs(Field field, Grid3d& gr, std::string nameFile);
void WriteFileField(Field field, Coords coord, Grid3d& gr, std::string nameFile);