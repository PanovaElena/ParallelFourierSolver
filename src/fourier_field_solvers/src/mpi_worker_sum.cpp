#include "mpi_worker_sum.h"
#include "fourier_transformation.h"
#include "class_member_ptr.h"

void MPIWorkerSum::ApplyMask() {
    for (int i = 0; i < grid.gnxRealCells(); i++)
        for (int j = 0; j < grid.gnyRealCells(); j++)
            for (int k = 0; k < grid.gnzRealCells(); k++) {
                grid.E.Write(i, j, k, grid.E(i, j, k) * mask(vec3<int>(i, j, k), getMainDomainSize(), getGuardSize()));
                grid.B.Write(i, j, k, grid.B(i, j, k) * mask(vec3<int>(i, j, k), getMainDomainSize(), getGuardSize()));
                grid.J.Write(i, j, k, grid.J(i, j, k) * mask(vec3<int>(i, j, k), getMainDomainSize(), getGuardSize()));
            }
}


void MPIWorkerSum::UnPackData(vec3<int> n1, vec3<int> n2, double *& arr, Grid3d& grTo)
{
    int num = 0;
    for (int i = n1.x; i < n2.x; i++)
        for (int j = n1.y; j < n2.y; j++)
            for (int k = n1.z; k < n2.z; k++)
                for (int coord = 0; coord < 3; coord++) {
                    grTo.E[coord](i, j, k) += arr[num++];
                    grTo.B[coord](i, j, k) += arr[num++];
                }
}

void MPIWorkerSum::getBoardsForExchange(int& sl1, int& sl2, int& sr1, int& sr2,
    int& rl1, int& rl2, int& rr1, int& rr2, Coordinate coord)
{
    sl1 = 0;
    sl2 = 2 * getGuardSize().*GetCoord<int>(coord);
    sr1 = getMainDomainSize().*GetCoord<int>(coord);
    sr2 = getFullDomainSize().*GetCoord<int>(coord);
    rr1 = getMainDomainSize().*GetCoord<int>(coord);
    rr2 = getFullDomainSize().*GetCoord<int>(coord);
    rl1 = 0;
    rl2 = 2 * getGuardSize().*GetCoord<int>(coord);
}
