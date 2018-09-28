#include "field_solver.h"
#include "grid3d.h"
#include "physical_constants.h"
#include "fourier_transformation.h"

int mod(int a, int b) {
    return (a + b) % b;
}

void RefreshE(Grid3d& gr, double dt) {
	for (int i = 0; i < gr.gnxComplexCells(); i++)
		for (int j = 0; j < gr.gnyComplexCells(); j++)
			for (int k = 0; k < gr.gnzComplexCells(); k++) {

                double ox = OmegaX(i, gr);
                double oy = OmegaY(j, gr);
                double oz = OmegaZ(k, gr);

                gr(i, j, k).EF[0] += complex_i*constants::c*dt*(oy*gr(i, j, k).BF.z() - oz*gr(i, j, k).BF.y());
                gr(i, j, k).EF[1] -= complex_i*constants::c*dt*(ox*gr(i, j, k).BF.z() - oz*gr(i, j, k).BF.x());
                gr(i, j, k).EF[2] += complex_i*constants::c*dt*(ox*gr(i, j, k).BF.y() - oy*gr(i, j, k).BF.x());

                gr(i, j, k).EF -= gr(i, j, k).JF;
            }
}

void RefreshB(Grid3d& gr, double dt) {
	for (int i = 0; i < gr.gnxComplexCells(); i++)
		for (int j = 0; j < gr.gnyComplexCells(); j++)
			for (int k = 0; k < gr.gnzComplexCells(); k++) {

                double ox = OmegaX(i, gr);
                double oy = OmegaY(j, gr);
                double oz = OmegaZ(k, gr);

                gr(i, j, k).BF[0] -= complex_i*constants::c*dt*(oy*gr(i, j, k).EF.z() - oz*gr(i, j, k).EF.y());
                gr(i, j, k).BF[1] += complex_i*constants::c*dt*(ox*gr(i, j, k).EF.z() - oz*gr(i, j, k).EF.x());
                gr(i, j, k).BF[2] -= complex_i*constants::c*dt*(ox*gr(i, j, k).EF.y() - oy*gr(i, j, k).EF.x());

            }
}

void FieldSolverPSTD(Grid3d & gr, double dt) {
    RefreshE(gr, dt);
    RefreshB(gr, dt);
}