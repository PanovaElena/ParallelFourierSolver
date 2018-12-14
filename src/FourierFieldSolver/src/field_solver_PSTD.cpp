#include "field_solver.h"
#include "grid3d.h"
#include "physical_constants.h"
#include "fourier_transformation.h"

void RefreshE_PSTD(Grid3d& gr, double dt) {

    for (int i = 0; i < gr.gnxComplexCells(); i++)
        for (int j = 0; j < gr.gnyComplexCells(); j++)
            for (int k = 0; k < gr.gnzComplexCells(); k++) {

                vec3<MyComplex> K = GetK(vec3<int>(i, j, k), gr);

                gr.EF.Write(i, j, k, gr.EF(i, j, k) + complex_i*constants::c*dt*
                    vec3<MyComplex>::VectorProduct(K, gr.BF(i, j, k)) - 4 * constants::pi * gr.JF(i, j, k) * dt);
            }
}

void RefreshB_PSTD(Grid3d& gr, double dt) {

    for (int i = 0; i < gr.gnxComplexCells(); i++)
        for (int j = 0; j < gr.gnyComplexCells(); j++)
            for (int k = 0; k < gr.gnzComplexCells(); k++) {

                vec3<MyComplex> K = GetK(vec3<int>(i, j, k), gr);

                gr.BF.Write(i, j, k, gr.BF(i, j, k) - complex_i*constants::c*dt*
                    vec3<MyComplex>::VectorProduct(K, gr.EF(i, j, k)));
            }
}

void fieldSolverPSTD(Grid3d & gr, double dt) {
    RefreshE_PSTD(gr, dt);
    RefreshB_PSTD(gr, dt);
}