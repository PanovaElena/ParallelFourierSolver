#include "mpi_worker.h"

void MPIWorker::setLeftGuardStart(int guardWidth, Grid3d & gr)
{
    if (rank == 0)
        leftGuardStart = gr.gnxRealNodes() - guardWidth;
    else leftGuardStart = getMainDomainStart() - guardWidth;
}

void MPIWorker::setRightGuardStart(int guardWidth, Grid3d & gr)
{
    if (rank == size - 1)
        rightGuardStart = 0;
    else rightGuardStart = getMainDomainEnd();
}

void MPIWorker::CreateGrid(Grid3d & gr) 
{
    double a = getLeftGuardStart()*gr.gdx(), b = gr.gdx()*getFullDomainSize() + a;
    grid = Grid3d(getFullDomainSize(), gr.gnyRealCells(), gr.gnyRealCells(), a, b, gr.gay(), gr.gby(), gr.gaz(), gr.gbz());
    for (int i = 0; i <= getFullDomainSize(); i++)
        for (int j = 0; j < gr.gnyRealNodes(); j++)
            for (int k = 0; k < gr.gnzRealNodes(); k++)
                grid(i, j, k) = gr(mod(i + getLeftGuardStart(), gr.gnxRealCells()), j, k);
}

void MPIWorker::SendGuard(double*& arr, int& size, int n1, int n2, int dest)
{
    PackData(grid, n1, n2, arr, size);
    MPIWrapper::MPISend(arr, size, dest);
}

void MPIWorker::RecvGuard(double*& arr, int& size, int n1, int n2, int source)
{
    MPIWrapper::MPIRecv(arr, size, source);
    UnPackData(grid, n1, n2, arr);
}

void MPIWorker::ExchangeGuard()
{
    double* arrLeft, *arrRight;
    int sizeLeft, sizeRight;
    SendGuard(arrLeft, sizeLeft, 0, getGuardSize(), mod(rank - 1, size));
    SendGuard(arrRight, sizeRight, getGuardSize() + getMainDomainSize(), getFullDomainSize(), mod(rank + 1, size));
    RecvGuard(arrLeft, sizeLeft, 0, getGuardSize(), mod(rank - 1, size));
    RecvGuard(arrRight, sizeRight, getGuardSize() + getMainDomainSize(), getFullDomainSize(), mod(rank + 1, size));
}

void MPIWorker::SetToZerosQuard()
{
    for (int i = 0; i <= guardSize; i++)
        for (int j = 0; j <= grid.gnyRealCells(); j++)
            for (int k = 0; j <= grid.gnzRealCells(); k++) {
                grid(i, j, k) = Node();
                grid(i + getGuardSize() + getMainDomainSize(), j, k) = Node();
            }
}

MPIWorker::MPIWorker(Grid3d & gr, double guardWidthPerc, int _size, int _rank)
{
    rank = _rank;
    size = _size;
    domainSize = gr.gnxRealCells() / size; //делится нацело
    domainStart = domainSize*rank;
    guardSize = (int)(guardWidthPerc*domainSize);
    setLeftGuardStart(guardSize, gr);
    setRightGuardStart(guardSize, gr);

    CreateGrid(gr);
}

void MPIWorker::PackData(Grid3d & gr, int n1, int n2, double *& arr, int & size)
{
    const int n = 2, d = 3; //2 вектора (B, E) из 3 компонент
    size = n*d*(n2 - n1 + 1)*gr.gnyRealNodes()*gr.gnzRealNodes();
    arr = new double[size];
    for (int i = n1; i < n2; i++)
        for (int j = 0; j < gr.gnyRealNodes(); j++)
            for (int k = 0; k < gr.gnzRealNodes(); k++)
                for (int coord = 0; i < 2; coord++) {
                    arr[n*d*(i*gr.gnyRealNodes()*gr.gnzRealNodes() + j*gr.gnzRealNodes() + k) + n*coord + 0] = gr(i, j, k).E[coord];
                    arr[n*d*(i*gr.gnyRealNodes()*gr.gnzRealNodes() + j*gr.gnzRealNodes() + k) + n*coord + 1] = gr(i, j, k).B[coord];
                }
}

void MPIWorker::UnPackData(Grid3d & gr, int n1, int n2, double * arr)
{
    const int n = 2, d = 3; //2 вектора (B, E) из 3 компонент
    for (int i = n1; i < n2; i++)
        for (int j = 0; j < gr.gnyRealNodes(); j++)
            for (int k = 0; k < gr.gnzRealNodes(); k++)
                for (int coord = 0; i < 2; coord++) {
                    gr(i, j, k).E[coord] += arr[n*d*(i*gr.gnyRealNodes()*gr.gnzRealNodes() + j*gr.gnzRealNodes() + k) + n*coord + 0];
                    gr(i, j, k).B[coord] += arr[n*d*(i*gr.gnyRealNodes()*gr.gnzRealNodes() + j*gr.gnzRealNodes() + k) + n*coord + 1];
                }
    delete[] arr;
}
