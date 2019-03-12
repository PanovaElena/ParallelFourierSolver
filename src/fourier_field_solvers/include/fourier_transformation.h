#pragma once
#include "vector3d.h"
#include "my_complex.h"
#include "physical_constants.h"
#include "grid3d.h"
#include "simple_types.h"

void fourierTransformation(Grid3d& gr, Direction dir);
void fourierTransformation(Grid3d& gr, Field _field, Coordinate _coord, Direction dir);

inline vec3<MyComplex> getFreqVector(const vec3<int>& ind, Grid3d& gr) {
    MyComplex v1 = (2 * constants::pi*((ind.x <= gr.sizeReal().x / 2) ? ind.x : ind.x - gr.sizeReal().x)) / (gr.getEnd().x - gr.getStart().x);
    MyComplex v2 = (2 * constants::pi*((ind.y <= gr.sizeReal().y / 2) ? ind.y : ind.y - gr.sizeReal().y)) / (gr.getEnd().y - gr.getStart().y);
    MyComplex v3 = (2 * constants::pi*((ind.z <= gr.sizeReal().z / 2) ? ind.z : ind.z - gr.sizeReal().z)) / (gr.getEnd().z - gr.getStart().z);
    return vec3<MyComplex>(v1, v2, v3);
}
