#include "field_solver.h"
#include "grid2d.h"
#include "constants.h"

int mod(int a, int b) {
	return (a + b) % b;
}

void RefreshE_diff(Grid2d& gr, double dt) {
	double _x, _y, _z;

	for (int i = 0; i<=gr.gnx(); i++)
		for (int j = 0; j <= gr.gny(); j++) {
			int nextI = mod(i + 1, gr.gnx());
			int prevI = mod(i - 1, gr.gnx());
			int nextJ = mod(j + 1, gr.gny());
			int prevJ = mod(j - 1, gr.gny());

			_x = gr(i, j).E.x - 4 * constants::pi*dt*gr(i, j).J.x + constants::c*dt* (gr(i, nextJ).B.z - gr(i, prevJ).B.z) / (2 * gr.gdy());
			_y = gr(i, j).E.y - 4 * constants::pi*dt*gr(i, j).J.y - constants::c*dt* (gr(nextI, j).B.z - gr(prevI, j).B.z) / (2 * gr.gdx());
			_z = gr(i, j).E.z - 4 * constants::pi*dt*gr(i, j).J.z + constants::c*dt* ((gr(nextI, j).B.y - gr(prevI, j).B.y) / (2 * gr.gdx()) - (gr(i, nextJ).B.x - gr(i, prevJ).B.x) / (2 * gr.gdy()));
			gr(i,j).E = vec3(_x, _y, _z);
		}
}
void RefreshB_diff(Grid2d& gr, double dt) {
	double _x, _y, _z;

	for (int i = 0; i<=gr.gnx(); i++)
		for (int j = 0; j <= gr.gny(); j++) {
			int nextI = mod(i + 1, gr.gnx());
			int prevI = mod(i - 1, gr.gnx());
			int nextJ = mod(j + 1, gr.gny());
			int prevJ = mod(j - 1, gr.gny());

			_x = gr(i, j).B.x - constants::c*dt* (gr(i, nextJ).E.z - gr(i,  prevJ).E.z) / (2 * gr.gdy());
			_y = gr(i, j).B.y + constants::c*dt* (gr(nextI, j).E.z - gr(prevI, j).E.z) / (2 * gr.gdx());
			_z = gr(i, j).B.z - constants::c*dt* ((gr(nextI, j).E.y - gr(prevI, j).E.y) / (2 * gr.gdx()) - (gr(i, nextJ).E.x - gr(i, prevJ).E.x) / (2 * gr.gdy()));
			gr(i,j).B = vec3(_x, _y, _z);
		}
}

void FieldSolver_diff(Grid2d& gr, double dt) {
	RefreshE_diff(gr, dt);
	RefreshB_diff(gr, dt);
}
