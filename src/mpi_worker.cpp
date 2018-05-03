#include "mpi_worker.h"
#include "fourier_transformation.h"

void MPIWorker::setLeftGuardStart(int guardWidth, Grid3d & gr)
{
    if (rank == 0)
        leftGuardStart = gr.gnxRealCells() - guardWidth;
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
    SetToZerosQuard();
    FourierTransformation(gr, RtoC);
}

void MPIWorker::Send(int n1, int n2, int dest, int tag, Grid3d& grFrom)
{
    double* arr = new double[getPackSize(n2 - n1)];
    PackData(n1, n2, arr, grFrom);
    MPIWrapper::MPISend(arr, getPackSize(n2 - n1), dest, tag);
    delete[] arr;
}

void MPIWorker::Recv(int n1, int n2, int source, int tag, Grid3d& grTo)
{
    double* arr = new double[getPackSize(n2 - n1)];
    MPIWrapper::MPIRecv(arr, getPackSize(n2 - n1), source, tag);
    UnPackData(n1, n2, arr, grTo);
    delete[] arr;
}

void MPIWorker::ExchangeGuard()
{
    Send(0, 0 + getGuardSize(), mod(rank - 1, size), 0, grid);
    Send(getGuardSize() + getMainDomainSize(), 2 * getGuardSize() + getMainDomainSize(), mod(rank + 1, size), 1, grid);
    Recv(getGuardSize() + getMainDomainSize(), 2 * getGuardSize() + getMainDomainSize(), mod(rank + 1, size), 0, grid);
    Recv(0, 0 + getGuardSize(), mod(rank - 1, size), 1, grid);
}

void MPIWorker::AssembleResultsToZeroProcess(Grid3d& gr)
{
    if (MPIWrapper::MPIRank() != 0)
        Send(getGuardSize() + 1, getGuardSize() + getMainDomainSize(), 0, 2, grid);

    if (MPIWrapper::MPIRank() == 0) {
        gr = Grid3d(gr.gnxRealCells(), gr.gnyRealCells(), gr.gnzRealCells(), gr.gax(), gr.gbx(), gr.gay(), gr.gby(), gr.gaz(), gr.gbz());

        for (int i = 0; i <= getMainDomainSize(); i++)
            for (int j = 0; j < gr.gnyRealNodes(); j++)
                for (int k = 0; k < gr.gnzRealNodes(); k++)
                    gr(i, j, k) = grid(i + getGuardSize(), j, k);

        for (int r = 1; r < MPIWrapper::MPISize(); r++) {
            Recv(r*getMainDomainSize() + 1, (r + 1)*getMainDomainSize(), r, 2, gr);
            ShowMessage("recv from " + std::to_string(r));
        }
    }
}

void MPIWorker::SetToZerosQuard()
{
    for (int i = 0; i <= getGuardSize(); i++)
        for (int j = 0; j < grid.gnyRealNodes(); j++)
            for (int k = 0; k < grid.gnzRealNodes(); k++) {
                grid(i, j, k) = Node();
                grid(i + getGuardSize() + getMainDomainSize(), j, k) = Node();
            }
}

MPIWorker::MPIWorker(Grid3d & gr, int guardWidth, int _size, int _rank)
{
    rank = _rank;
    size = _size;
    domainSize = gr.gnxRealCells() / size; //делится нацело
    domainStart = domainSize*rank;
    guardSize = guardWidth;
    setLeftGuardStart(guardSize, gr);
    setRightGuardStart(guardSize, gr);

    CreateGrid(gr);
}

int MPIWorker::getPackSize(int width)
{
    return n*d*(width +1)*grid.gnyRealNodes()*grid.gnzRealNodes();
}

void MPIWorker::PackData(int n1, int n2, double *& arr, Grid3d& grFrom)
{
    for (int i = n1; i <= n2; i++)
        for (int j = 0; j < grFrom.gnyRealNodes(); j++)
            for (int k = 0; k < grFrom.gnzRealNodes(); k++)
                for (int coord = 0; coord < 3; coord++) {
                    int num = n*d*((i - n1)*grFrom.gnyRealNodes()*grFrom.gnzRealNodes() + j*grFrom.gnzRealNodes() + k) + n*coord;
                    arr[num + 0] = grFrom(i, j, k).E[coord];
                    arr[num + 1] = grFrom(i, j, k).B[coord];
                }
}

void MPIWorker::UnPackData(int n1, int n2, double *& arr, Grid3d& grTo)
{
    for (int i = n1; i <= n2; i++)
        for (int j = 0; j < grTo.gnyRealNodes(); j++)
            for (int k = 0; k < grTo.gnzRealNodes(); k++)
                for (int coord = 0; coord < 3; coord++) {
                    int num = n*d*((i - n1)*grTo.gnyRealNodes()*grTo.gnzRealNodes() + j*grTo.gnzRealNodes() + k) + n*coord;
                    grTo(i, j, k).E[coord] += arr[num + 0];
                    grTo(i, j, k).B[coord] += arr[num + 1];
                }
}
