#pragma once
#include "mpi_worker.h"

class MPIWorkerSum : public MPIWorker {
public:
    MPIWorkerSum() {}
    MPIWorkerSum(Grid3d& gr, vec3<int> guardWidth, Mask mask, const MPIWrapper3d& _mpiWrapper3d) {
        initialize(gr, guardWidth, mask, _mpiWrapper3d);
    }
    MPIWorkerSum(Grid3d& gr, vec3<int> guardWidth, Mask mask, int _size, int _rank) {
        initialize(gr, guardWidth, mask, _size, _rank);
    }
    MPIWorkerSum(vec3<int> commonSize, vec3<int> guardWidth, const Mask& mask,
        const MPIWrapper3d& _mpiWrapper3d, const StartConditions& startConditions) {
        initialize(commonSize, guardWidth, mask, _mpiWrapper3d, startConditions);
    }

    void applyMask(Field f, Coordinate c) override;

protected:
    void getBoardsForExchange(int& sl1, int& sl2, int& sr1, int& sr2,
        int& rl1, int& rl2, int& rr1, int& rr2, Coordinate coord) override;

    void unpackData(vec3<int> n1, vec3<int> n2, double*& arr, Grid3d& grTo) override;

    Status send(vec3<int> n1, vec3<int> n2, double*& arr, vec3<int> dest, int tag, Grid3d& grFrom,
        MPI_Request& request) override;
    Status recv(vec3<int> n1, vec3<int> n2, vec3<int> source, int tag, Grid3d& grTo) override;

};