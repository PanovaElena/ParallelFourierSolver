#include "field_solver.h"
#include "grid3d.h"
#include "physical_constants.h"

void refreshE_FDTD(Grid3d& gr, double dt) {
    double _x, _y, _z;
#pragma omp parallel for
    for (int i = 0; i < gr.sizeReal().x; i++)
        for (int j = 0; j < gr.sizeReal().y; j++)
            for (int k = 0; k < gr.sizeReal().z; k++) {

                int prevI = mod(i - 1, gr.sizeReal().x);
                int prevJ = mod(j - 1, gr.sizeReal().y);
                int prevK = mod(k - 1, gr.sizeReal().z);

                _x = gr.E(i, j, k).x + constants::c*dt*
                    ((gr.B(i, j, k).z - gr.B(i, prevJ, k).z) / (gr.getStep().y) -
                    (gr.B(i, j, k).y - gr.B(i, j, prevK).y) / (gr.getStep().z));
                _y = gr.E(i, j, k).y - constants::c*dt*
                    ((gr.B(i, j, k).z - gr.B(prevI, j, k).z) / (gr.getStep().x) -
                    (gr.B(i, j, k).x - gr.B(i, j, prevK).x) / (gr.getStep().z));
                _z = gr.E(i, j, k).z + constants::c*dt*
                    ((gr.B(i, j, k).y - gr.B(prevI, j, k).y) / (gr.getStep().x) -
                    (gr.B(i, j, k).x - gr.B(i, prevJ, k).x) / (gr.getStep().y));

                vec3<double> res(_x, _y, _z);
                gr.E.write(i, j, k, res - 4 * constants::pi*gr.J(i, j, k) * dt);
            }
}

void refreshB_FDTD(Grid3d& gr, double dt) {
    double _x, _y, _z;
#pragma omp parallel for
    for (int i = 0; i < gr.sizeReal().x; i++)
        for (int j = 0; j < gr.sizeReal().y; j++)
            for (int k = 0; k < gr.sizeReal().z; k++) {

                int nextI = mod(i + 1, gr.sizeReal().x);
                int nextJ = mod(j + 1, gr.sizeReal().y);
                int nextK = mod(k + 1, gr.sizeReal().z);

                _x = gr.B(i, j, k).x - constants::c*dt*
                    ((gr.E(i, nextJ, k).z - gr.E(i, j, k).z) / (gr.getStep().y) -
                    (gr.E(i, j, nextK).y - gr.E(i, j, k).y) / (gr.getStep().z));
                _y = gr.B(i, j, k).y + constants::c*dt*
                    ((gr.E(nextI, j, k).z - gr.E(i, j, k).z) / (gr.getStep().x) -
                    (gr.E(i, j, nextK).x - gr.E(i, j, k).x) / (gr.getStep().z));
                _z = gr.B(i, j, k).z - constants::c*dt*
                    ((gr.E(nextI, j, k).y - gr.E(i, j, k).y) / (gr.getStep().x) -
                    (gr.E(i, nextJ, k).x - gr.E(i, j, k).x) / (gr.getStep().y));

                vec3<double> res(_x, _y, _z);
                gr.B.write(i, j, k, res);
            }
}

void fieldSolverFDTD(Grid3d& gr, double dt) {
    refreshB_FDTD(gr, dt);
    refreshE_FDTD(gr, dt);
}