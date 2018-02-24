#include "field_solver.h"
#include "grid3d.h"
#include "constants.h"

int mod(int a, int b) {
	return (a + b) % b;
}

inline double OmegaX(int i, const Grid3d& gr) {
	return (2 * constants::pi*((i <= gr.gnxCells() / 2) ? i : i - gr.gnxCells())) / (gr.gbx() - gr.gax());
}
inline double OmegaY(int j, const Grid3d& gr) {
	return (2 * constants::pi*((j <= gr.gnyCells() / 2) ? j : j - gr.gnyCells())) / (gr.gby() - gr.gay());
}
inline double OmegaZ(int k, const Grid3d& gr) {
	return (2 * constants::pi*((k <= gr.gnzCells() / 2) ? k : k - gr.gnzCells())) / (gr.gbz() - gr.gaz());
}

void RefreshE(Grid3d& gr, double dt) {
	for (int i = 0; i < gr.gnxNodes(); i++)
		for (int j = 0; j < gr.gnyNodes(); j++)
			for (int k = 0; k < gr.gnzNodes()/2+1; k++) {

				double ox = OmegaX(i, gr);
				double oy = OmegaY(j, gr);
				double oz = OmegaZ(k, gr);

				//gr(i, j, k).EF[0] += complex_i*constants::c*dt*(oy*gr(i, j, k).BF.z() - oz*gr(i, j, k).BF.y());
				gr(i, j, k).EF[1] -= complex_i*constants::c*dt*(ox*gr(i, j, k).BF.z()/* - oz*gr(i, j, k).BF.x()*/);
				//gr(i, j, k).EF[2] += complex_i*constants::c*dt*(ox*gr(i, j, k).BF.y() - oy*gr(i, j, k).BF.x());

			}
}

void RefreshB(Grid3d& gr, double dt) {
	for (int i = 0; i < gr.gnxNodes(); i++)
		for (int j = 0; j < gr.gnyNodes(); j++)
			for (int k = 0; k < gr.gnzNodes(); k++) {

				double ox = OmegaX(i, gr);
				double oy = OmegaY(j, gr);
				double oz = OmegaZ(k, gr);

				//gr(i, j, k).BF[0] -= complex_i*constants::c*dt*(oy*gr(i, j, k).EF.z() - oz*gr(i, j, k).EF.y());
				//gr(i, j, k).BF[1] += complex_i*constants::c*dt*(ox*gr(i, j, k).EF.z() - oz*gr(i, j, k).EF.x());
				gr(i, j, k).BF[2] -= complex_i*constants::c*dt*(ox*gr(i, j, k).EF.y()/* - oy*gr(i, j, k).EF.x()*/);

			}
}

void FieldSolver(Grid3d & gr, double dt) {
	RefreshE(gr, dt);
	RefreshB(gr, dt);
}