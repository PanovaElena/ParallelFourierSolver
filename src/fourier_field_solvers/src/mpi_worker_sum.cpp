#include "mpi_worker_sum.h"
#include "class_member_ptr.h"

void MPIWorkerSum::applyMask(Field f, Coordinate c) {
#pragma omp parallel for
    for (int i = 0; i < grid.sizeReal().x; i++)
        for (int j = 0; j < grid.sizeReal().y; j++)
#pragma omp simd
            for (int k = 0; k < grid.sizeReal().z; k++)
                ((grid.*getMemberPtrField<double>(f)).*getMemberPtrFieldCoord<double>(c))(i, j, k) *=
                    mask(vec3<int>(i, j, k), getMainDomainSize(), getGuardSize());
}

void MPIWorkerSum::unpackData(vec3<int> n1, vec3<int> n2, double *& arr, Grid3d& grTo) {
    int num = 0;
    for (int coord = 0; coord < 3; coord++) {
#pragma omp parallel for
        for (int i = n1.x; i < n2.x; i++)
            for (int j = n1.y; j < n2.y; j++)
#pragma omp simd
                for (int k = n1.z; k < n2.z; k++)
                    grTo.E[coord](i, j, k) += arr[num++];

#pragma omp parallel for
        for (int i = n1.x; i < n2.x; i++)
            for (int j = n1.y; j < n2.y; j++)
#pragma omp simd
                for (int k = n1.z; k < n2.z; k++)
                    grTo.B[coord](i, j, k) += arr[num++];
    }
}

void MPIWorkerSum::getBoardsForExchange(int& sl1, int& sl2, int& sr1, int& sr2,
    int& rl1, int& rl2, int& rr1, int& rr2, Coordinate coord) {
    sl1 = 0;
    sl2 = 2 * getGuardSize().*getMemberPtrCoord<int>(coord);
    sr1 = getMainDomainSize().*getMemberPtrCoord<int>(coord);
    sr2 = getFullDomainSize().*getMemberPtrCoord<int>(coord);
    rr1 = getMainDomainSize().*getMemberPtrCoord<int>(coord);
    rr2 = getFullDomainSize().*getMemberPtrCoord<int>(coord);
    rl1 = 0;
    rl2 = 2 * getGuardSize().*getMemberPtrCoord<int>(coord);
}

Status MPIWorkerSum::send(vec3<int> n1, vec3<int> n2, double *& arr, vec3<int> dest,
    int tag, Grid3d & grFrom, MPI_Request & request) {
    if (n1.x > n2.x || n1.y > n2.y || n1.z > n2.z) return Status::STOP;
    arr = new double[getPackSize(n1, n2)];
    packData(n1, n2, arr, grFrom);
    mpiWrapper3d.MPIISend(arr, getPackSize(n1, n2), dest, tag, request);
    return Status::OK;
}

Status MPIWorkerSum::recv(vec3<int> n1, vec3<int> n2, vec3<int> source, int tag, Grid3d& grTo) {
    if (n1.x > n2.x || n1.y > n2.y || n1.z > n2.z) return Status::ERROR;
    double* arr = new double[getPackSize(n1, n2)];
    mpiWrapper3d.MPIRecv(arr, getPackSize(n1, n2), source, tag);
    unpackData(n1, n2, arr, grTo);
    if (arr) delete[] arr;
    return Status::OK;
}
