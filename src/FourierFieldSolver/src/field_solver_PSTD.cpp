#include "field_solver.h"
#include "grid3d.h"
#include "physical_constants.h"
#include "fourier_transformation.h"

void RefreshE_PSTD(Grid3d& gr, double dt) {
    for (int i = 0; i < gr.gnxComplexCells(); i++)
        for (int j = 0; j < gr.gnyComplexCells(); j++)
            for (int k = 0; k < gr.gnzComplexCells(); k++) {

                vec3<MyComplex> omega(OmegaX(i, gr), OmegaY(j, gr), OmegaZ(k, gr));

                gr(i, j, k).EF += complex_i*constants::c*dt*vec3<MyComplex>::VectorProduct(omega, gr(i, j, k).BF);

                gr(i, j, k).EF -= 4 * constants::pi*gr(i, j, k).JF * dt;

            }
}

void RefreshB_PSTD(Grid3d& gr, double dt) {
	for (int i = 0; i < gr.gnxComplexCells(); i++)
		for (int j = 0; j < gr.gnyComplexCells(); j++)
			for (int k = 0; k < gr.gnzComplexCells(); k++) {

                vec3<MyComplex> omega(OmegaX(i, gr), OmegaY(j, gr), OmegaZ(k, gr));

                gr(i, j, k).BF -= complex_i*constants::c*dt*vec3<MyComplex>::VectorProduct(omega, gr(i, j, k).EF);

            }
}

void FieldSolverPSTD(Grid3d & gr, double dt) {
    RefreshE_PSTD(gr, dt);
    RefreshB_PSTD(gr, dt);
}