#pragma once
#include "vec.h"
#include "my_complex.h"
#include "physical_constants.h"
#include "grid3d.h"
#include "simple_types_and_constants.h"

void FourierTransformation(Grid3d& gr, Direction dir);
void FourierTransformation(Grid3d& gr, Field _field, Coordinate _coord, Direction dir);

inline vec3<MyComplex> GetK(const vec3<int>& ind, Grid3d& gr) {
    MyComplex v1 = (2 * constants::pi*((ind.x <= gr.gnxRealCells() / 2) ? ind.x : ind.x - gr.gnxRealCells())) / (gr.gbx() - gr.gax());
    MyComplex v2 = (2 * constants::pi*((ind.y <= gr.gnyRealCells() / 2) ? ind.y : ind.y - gr.gnyRealCells())) / (gr.gby() - gr.gay());
    MyComplex v3 = (2 * constants::pi*((ind.z <= gr.gnzRealCells() / 2) ? ind.z : ind.z - gr.gnzRealCells())) / (gr.gbz() - gr.gaz());
    return vec3<MyComplex>(v1, v2, v3);
}
