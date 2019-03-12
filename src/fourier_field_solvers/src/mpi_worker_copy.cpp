#include "mpi_worker_copy.h"
#include "class_member_ptr.h"

void MPIWorkerCopy::applyMask() {
    for (int i = 0; i < grid.sizeReal().x; i++)
        for (int j = 0; j < grid.sizeReal().y; j++)
            for (int k = 0; k < grid.sizeReal().z; k++) {
                grid.E.write(i, j, k, grid.E(i, j, k) * mask(vec3<int>(i, j, k), getMainDomainSize() + getGuardSize(), getGuardSize() / 2));
                grid.B.write(i, j, k, grid.B(i, j, k) * mask(vec3<int>(i, j, k), getMainDomainSize() + getGuardSize(), getGuardSize() / 2));
                grid.J.write(i, j, k, grid.J(i, j, k) * mask(vec3<int>(i, j, k), getMainDomainSize() + getGuardSize(), getGuardSize() / 2));
            }
}


void MPIWorkerCopy::unpackData(vec3<int> n1, vec3<int> n2, double *& arr, Grid3d& grTo) {
    int num = 0;
    for (int i = n1.x; i < n2.x; i++)
        for (int j = n1.y; j < n2.y; j++)
            for (int k = n1.z; k < n2.z; k++)
                for (int coord = 0; coord < 3; coord++) {
                    grTo.E[coord](i, j, k) = arr[num++];
                    grTo.B[coord](i, j, k) = arr[num++];
                }
}

void MPIWorkerCopy::getBoardsForExchange(int& sl1, int& sl2, int& sr1, int& sr2,
    int& rl1, int& rl2, int& rr1, int& rr2, Coordinate coord) {
    sl1 = getGuardSize().*getMemberPtrCoord<int>(coord);
    sl2 = 2 * getGuardSize().*getMemberPtrCoord<int>(coord);
    sr1 = getMainDomainSize().*getMemberPtrCoord<int>(coord);
    sr2 = getFullDomainSize().*getMemberPtrCoord<int>(coord) - getGuardSize().*getMemberPtrCoord<int>(coord);
    rr1 = getFullDomainSize().*getMemberPtrCoord<int>(coord) - getGuardSize().*getMemberPtrCoord<int>(coord);
    rr2 = getFullDomainSize().*getMemberPtrCoord<int>(coord);
    rl1 = 0;
    rl2 = getGuardSize().*getMemberPtrCoord<int>(coord);
}
