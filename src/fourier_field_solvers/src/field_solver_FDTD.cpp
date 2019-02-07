#include "field_solver.h"
#include "grid3d.h"
#include "physical_constants.h"

void RefreshE_FDTD(Grid3d& gr, double dt) {
    double _x, _y, _z;
    for (int i = 0; i < gr.gnxRealCells(); i++)
        for (int j = 0; j < gr.gnyRealCells(); j++)
            for (int k = 0; k < gr.gnzRealCells(); k++) {

                int prevI = mod(i - 1, gr.gnxRealCells());
                int prevJ = mod(j - 1, gr.gnyRealCells());
                int prevK = mod(k - 1, gr.gnzRealCells());

                _x = gr.E(i, j, k).x + constants::c*dt*
                    ((gr.B(i, j, k).z - gr.B(i, prevJ, k).z) / (gr.gdy()) -
                    (gr.B(i, j, k).y - gr.B(i, j, prevK).y) / (gr.gdz()));
                _y = gr.E(i, j, k).y - constants::c*dt*
                    ((gr.B(i, j, k).z - gr.B(prevI, j, k).z) / (gr.gdx()) -
                    (gr.B(i, j, k).x - gr.B(i, j, prevK).x) / (gr.gdz()));
                _z = gr.E(i, j, k).z + constants::c*dt*
                    ((gr.B(i, j, k).y - gr.B(prevI, j, k).y) / (gr.gdx()) -
                    (gr.B(i, j, k).x - gr.B(i, prevJ, k).x) / (gr.gdy()));

                vec3<double> res(_x, _y, _z);
                gr.E.Write(i, j, k, res - 4 * constants::pi*gr.J(i, j, k) * dt);
            }
}
void RefreshB_FDTD(Grid3d& gr, double dt) {
    double _x, _y, _z;
    for (int i = 0; i < gr.gnxRealCells(); i++)
        for (int j = 0; j < gr.gnyRealCells(); j++)
            for (int k = 0; k < gr.gnzRealCells(); k++) {

                int nextI = mod(i + 1, gr.gnxRealCells());
                int nextJ = mod(j + 1, gr.gnyRealCells());
                int nextK = mod(k + 1, gr.gnzRealCells());

                _x = gr.B(i, j, k).x - constants::c*dt*
                    ((gr.E(i, nextJ, k).z - gr.E(i, j, k).z) / (gr.gdy()) -
                    (gr.E(i, j, nextK).y - gr.E(i, j, k).y) / (gr.gdz()));
                _y = gr.B(i, j, k).y + constants::c*dt*
                    ((gr.E(nextI, j, k).z - gr.E(i, j, k).z) / (gr.gdx()) -
                    (gr.E(i, j, nextK).x - gr.E(i, j, k).x) / (gr.gdz()));
                _z = gr.B(i, j, k).z - constants::c*dt*
                    ((gr.E(nextI, j, k).y - gr.E(i, j, k).y) / (gr.gdx()) -
                    (gr.E(i, nextJ, k).x - gr.E(i, j, k).x) / (gr.gdy()));

                vec3<double> res(_x, _y, _z);
                gr.B.Write(i, j, k, res);
            }
}
void fieldSolverFDTD(Grid3d& gr, double dt) {
    RefreshE_FDTD(gr, dt);
    RefreshB_FDTD(gr, dt);
}