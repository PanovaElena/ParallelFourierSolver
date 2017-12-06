#include "field_solver.h"
#include "grid3d.h"
#include "constants.h"

int mod(int a, int b) {
	return (a + b) % b;
}


void RefreshE(Grid3d& gr, double dt) {
	for (int i = 0; i <= gr.gnx(); i++)
		for (int j = 0; j <= gr.gny(); j++)
			for (int k = 0; k <= gr.gnz(); k++) {
				double omegaX = (2 * constants::pi*((i <= gr.gnx() / 2) ? i : i - gr.gnx())) / (gr.gdx()*gr.gnx());
				double omegaY = (2 * constants::pi*((j <= gr.gny() / 2) ? j : j - gr.gny())) / (gr.gdy()*gr.gny());
				double omegaZ = (2 * constants::pi*((k <= gr.gnz() / 2) ? k : k - gr.gnz())) / (gr.gdz()*gr.gnz());

				gr(i, j, k).EF[0] += complex_i*constants::c*dt*(omegaY*gr(i, j, k).BF.z() - omegaZ*gr(i, j, k).BF.y());
				gr(i, j, k).EF[1] -= complex_i*constants::c*dt*(omegaX*gr(i, j, k).BF.z() - omegaZ*gr(i, j, k).BF.x());
				gr(i, j, k).EF[2] += complex_i*constants::c*dt*(omegaX*gr(i, j, k).BF.y() - omegaY*gr(i, j, k).BF.x());


			}
}

void RefreshB(Grid3d& gr, double dt) {
	for (int i = 0; i <= gr.gnx(); i++)
		for (int j = 0; j <= gr.gny(); j++)
			for (int k = 0; k <= gr.gnz(); k++) {
				double omegaX = (2 * constants::pi*((i <= gr.gnx() / 2) ? i : i - gr.gnx())) / (gr.gdx()*gr.gnx());
				double omegaY = (2 * constants::pi*((j <= gr.gny() / 2) ? j : j - gr.gny())) / (gr.gdy()*gr.gny());
				double omegaZ = (2 * constants::pi*((k <= gr.gnz() / 2) ? k : k - gr.gnz())) / (gr.gdz()*gr.gnz());

				gr(i, j, k).BF[0] -= complex_i*constants::c*dt*(omegaY*gr(i, j, k).EF.z() - omegaZ*gr(i, j, k).EF.y());
				gr(i, j, k).BF[1] += complex_i*constants::c*dt*(omegaX*gr(i, j, k).EF.z() - omegaZ*gr(i, j, k).EF.x());
				gr(i, j, k).BF[2] -= complex_i*constants::c*dt*(omegaX*gr(i, j, k).EF.y() - omegaY*gr(i, j, k).EF.x());


			}
}

void FieldSolver(Grid3d & gr, double dt) {
	RefreshE(gr, dt);
	RefreshB(gr, dt);
}