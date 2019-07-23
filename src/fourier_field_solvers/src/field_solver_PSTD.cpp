#include "field_solver.h"
#include "grid3d.h"
#include "physical_constants.h"
#include "fourier_transform.h"

void refreshE_PSTD(Grid3d& gr, double dt) {
#pragma omp parallel for
    for (int i = 0; i < gr.sizeComplex().x; i++)
        for (int j = 0; j < gr.sizeComplex().y; j++)
#pragma omp simd
            for (int k = 0; k < gr.sizeComplex().z; k++) {

                vec3<MyComplex> K = getFreqVector(vec3<int>(i, j, k), gr);

                gr.EF.write(i, j, k, gr.EF(i, j, k) + complex_i * constants::c*dt*
                    vec3<MyComplex>::cross(K, gr.BF(i, j, k)) - 4 * constants::pi * gr.JF(i, j, k) * dt);
            }
}

void refreshB_PSTD(Grid3d& gr, double dt) {
#pragma omp parallel for
    for (int i = 0; i < gr.sizeComplex().x; i++)
        for (int j = 0; j < gr.sizeComplex().y; j++)
#pragma omp simd
            for (int k = 0; k < gr.sizeComplex().z; k++) {

                vec3<MyComplex> K = getFreqVector(vec3<int>(i, j, k), gr);

                gr.BF.write(i, j, k, gr.BF(i, j, k) - complex_i * constants::c*dt*
                    vec3<MyComplex>::cross(K, gr.EF(i, j, k)));
            }
}

void fieldSolverPSTD(Grid3d& gr, double dt) {
    refreshE_PSTD(gr, dt);
    refreshB_PSTD(gr, dt);
}