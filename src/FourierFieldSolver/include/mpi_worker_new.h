#pragma once
#include "mpi_worker.h"

class MPIWorkerNew : public MPIWorker {
public:

    MPIWorkerNew() {}
    MPIWorkerNew(Grid3d& gr, vec3<int> guardWidth, Mask mask, int maskWidth, MPIWrapper3d& _mpiWrapper3d) {
        Initialize(gr, guardWidth, mask, maskWidth, _mpiWrapper3d);
    }

    MPIWorkerNew(Grid3d& gr, vec3<int> guardWidth, Mask mask, int maskWidth, int _size, int _rank) {
        Initialize(gr, guardWidth, mask, maskWidth, _size, _rank);
    }

    void ApplyMask() override;

    double getPartGuard() override { return 0.4; }

protected:

    void getBoardsForExchange(int& sl1, int& sl2, int& sr1, int& sr2,
        int& rl1, int& rl2, int& rr1, int& rr2, Coordinate coord) override;

    void UnPackData(vec3<int> n1, vec3<int> n2, double *& arr, Grid3d& grTo) override;

};