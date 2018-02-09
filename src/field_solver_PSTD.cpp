#include "field_solver.h"
#include "grid3d.h"
#include "constants.h"

int mod(int a, int b) {
	return (a + b) % b;
}


void RefreshE(Grid3d& gr, double dt) {
	for (int i = 0; i <= gr.gnxCells(); i++)
		for (int j = 0; j <= gr.gnyCells(); j++)
			for (int k = 0; k <= gr.gnzCells(); k++) {
				double omegaX = (2 * constants::pi*((i <= gr.gnxCells() / 2) ? i : i - gr.gnxCells())) / (gr.gbx() - gr.gax());
				double omegaY = (2 * constants::pi*((j <= gr.gnyCells() / 2) ? j : j - gr.gnyCells())) / (gr.gby() - gr.gay());
				double omegaZ = (2 * constants::pi*((k <= gr.gnzCells() / 2) ? k : k - gr.gnzCells())) / (gr.gbz() - gr.gaz());

				gr(i, j, k).EF[0] += complex_i*constants::c*dt*(omegaY*gr(i, j, k).BF.z() - omegaZ*gr(i, j, k).BF.y());
				gr(i, j, k).EF[1] -= complex_i*constants::c*dt*(omegaX*gr(i, j, k).BF.z() - omegaZ*gr(i, j, k).BF.x());
				gr(i, j, k).EF[2] += complex_i*constants::c*dt*(omegaX*gr(i, j, k).BF.y() - omegaY*gr(i, j, k).BF.x());


			}
}

void RefreshB(Grid3d& gr, double dt) {
	for (int i = 0; i <= gr.gnxCells(); i++)
		for (int j = 0; j <= gr.gnyCells(); j++)
			for (int k = 0; k <= gr.gnzCells(); k++) {
				double omegaX = (2 * constants::pi*((i <= gr.gnxCells() / 2) ? i : i - gr.gnxCells())) / (gr.gbx() - gr.gax());
				double omegaY = (2 * constants::pi*((j <= gr.gnyCells() / 2) ? j : j - gr.gnyCells())) / (gr.gby() - gr.gay());
				double omegaZ = (2 * constants::pi*((k <= gr.gnzCells() / 2) ? k : k - gr.gnzCells())) / (gr.gbz() - gr.gaz());

				gr(i, j, k).BF[0] -= complex_i*constants::c*dt*(omegaY*gr(i, j, k).EF.z() - omegaZ*gr(i, j, k).EF.y());
				gr(i, j, k).BF[1] += complex_i*constants::c*dt*(omegaX*gr(i, j, k).EF.z() - omegaZ*gr(i, j, k).EF.x());
				gr(i, j, k).BF[2] -= complex_i*constants::c*dt*(omegaX*gr(i, j, k).EF.y() - omegaY*gr(i, j, k).EF.x());


			}
}

void FieldSolver(Grid3d & gr, double dt) {
	RefreshE(gr, dt);
	RefreshB(gr, dt);
}