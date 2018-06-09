#pragma once
#include "mpi_worker.h"
#include "pulse.h"

class MPIWorkerPulse : public MPIWorker {
public:
    int n = grid.gnxRealCells();

    double f(double y, double z) {
        return cos(y * 2 * constants::pi / (8 * grid.gdy())) *
            cos(z * 2 * constants::pi / (8 * grid.gdz()));
    }

    double getY(int j) {
        return 
    }

    virtual void DoAfterSeparation() {
        for (int j=0; j<=n; j++)
            for (int k = 0; k <= n; k++) {
                if (abs(grid(n / 2, j, k).E.z()) >= 1e-4) {

                }
            }
    }
};
