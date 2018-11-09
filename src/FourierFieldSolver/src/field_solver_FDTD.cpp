#include "field_solver.h"
#include "grid3d.h"
#include "physical_constants.h"

int mod_FDTD(int a, int b) {
    return (a + b) % b;
}

void RefreshE_FDTD(Grid3d& gr, double dt) {
    double _x, _y, _z;
    for (int i = 0; i <= gr.gnxRealCells(); i++)
        for (int j = 0; j <= gr.gnyRealCells(); j++)
            for (int k = 0; k <= gr.gnzRealCells(); k++) {

                int nextI = mod_FDTD(i + 1, gr.gnxRealCells());
                int prevI = mod_FDTD(i - 1, gr.gnxRealCells());
                int nextJ = mod_FDTD(j + 1, gr.gnyRealCells());
                int prevJ = mod_FDTD(j - 1, gr.gnyRealCells());
                int nextK = mod_FDTD(k + 1, gr.gnzRealCells());
                int prevK = mod_FDTD(k - 1, gr.gnzRealCells());

                _x = gr(i, j, k).E.x + constants::c*dt*
                    ((gr(i, nextJ, k).B.z - gr(i, prevJ, k).B.z) / (2 * gr.gdy()) -
                    (gr(i, j, nextK).B.y - gr(i, j, prevK).B.y) / (2 * gr.gdz()));

                _y = gr(i, j, k).E.y - constants::c*dt*
                    ((gr(nextI, j, k).B.z - gr(prevI, j, k).B.z) / (2 * gr.gdx()) -
                    (gr(i, j, nextK).B.x - gr(i, j, prevK).B.x) / (2 * gr.gdz()));

                _z = gr(i, j, k).E.z + constants::c*dt*
                    ((gr(nextI, j, k).B.y - gr(prevI, j, k).B.y) / (2 * gr.gdx()) -
                    (gr(i, nextJ, k).B.x - gr(i, prevJ, k).B.x) / (2 * gr.gdy()));

                gr(i, j, k).E = vec3<double>(_x, _y, _z);
                gr(i, j, k).E -= 4 * constants::pi*gr(i, j, k).J * dt;
            }
}

void RefreshB_FDTD(Grid3d& gr, double dt) {
    double _x, _y, _z;
    for (int i = 0; i <= gr.gnxRealCells(); i++)
        for (int j = 0; j <= gr.gnyRealCells(); j++)
            for (int k = 0; k <= gr.gnzRealCells(); k++) {

                int nextI = mod_FDTD(i + 1, gr.gnxRealCells());
                int prevI = mod_FDTD(i - 1, gr.gnxRealCells());
                int nextJ = mod_FDTD(j + 1, gr.gnyRealCells());
                int prevJ = mod_FDTD(j - 1, gr.gnyRealCells());
                int nextK = mod_FDTD(k + 1, gr.gnzRealCells());
                int prevK = mod_FDTD(k - 1, gr.gnzRealCells());

                _x = gr(i, j, k).B.x - constants::c*dt*
                    ((gr(i, nextJ, k).E.z - gr(i, prevJ, k).E.z) / (2 * gr.gdy()) -
                    (gr(i, j, nextK).E.y - gr(i, j, prevK).E.y) / (2 * gr.gdz()));

                _y = gr(i, j, k).B.y + constants::c*dt*
                    ((gr(nextI, j, k).E.z - gr(prevI, j, k).E.z) / (2 * gr.gdx()) -
                    (gr(i, j, nextK).E.x - gr(i, j, prevK).E.x) / (2 * gr.gdz()));

                _z = gr(i, j, k).B.z - constants::c*dt*
                    ((gr(nextI, j, k).E.y - gr(prevI, j, k).E.y) / (2 * gr.gdx()) -
                    (gr(i, nextJ, k).E.x - gr(i, prevJ, k).E.x) / (2 * gr.gdy()));

                gr(i, j, k).B = vec3<double>(_x, _y, _z);
            }
}

void FieldSolverFDTD(Grid3d& gr, double dt) {
    RefreshE_FDTD(gr, dt);
    RefreshB_FDTD(gr, dt);
}