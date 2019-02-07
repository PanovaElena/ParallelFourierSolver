#pragma once
#include "mpi_worker.h"

class MPIWorkerSum : public MPIWorker {
public:

    MPIWorkerSum() {}
    MPIWorkerSum(Grid3d& gr, vec3<int> guardWidth, Mask mask, MPIWrapper3d& _mpiWrapper3d) {
        Initialize(gr, guardWidth, mask, _mpiWrapper3d);
    }

    MPIWorkerSum(Grid3d& gr, vec3<int> guardWidth, Mask mask, int _size, int _rank) {
        Initialize(gr, guardWidth, mask, _size, _rank);
    }

    void ApplyMask() override;

protected:

    void getBoardsForExchange(int& sl1, int& sl2, int& sr1, int& sr2,
        int& rl1, int& rl2, int& rr1, int& rr2, Coordinate coord) override;

    void UnPackData(vec3<int> n1, vec3<int> n2, double *& arr, Grid3d& grTo) override;

};