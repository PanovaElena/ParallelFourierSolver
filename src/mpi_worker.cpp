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
    rightGuardStart = getMainDomainEnd();
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

void MPIWorker::Send(int n1, int n2, double*& arr, int dest, int tag, Grid3d& grFrom, MPI_Request& request)
{
    arr = new double[getPackSize(n2 - n1)];
    PackData(n1, n2, arr, grFrom);
    MPIWrapper::MPIISend(arr, getPackSize(n2 - n1), dest, tag, request);
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
    double* arrS1, *arrS2;
    MPI_Request request1, request2;

    // неблокирующий send и блокирующий recv
    Send(0, 0 + getGuardSize() - 1, arrS1, mod(rank - 1, size), 0, grid, request1);
    Send(getGuardSize() + getMainDomainSize() + 1, getFullDomainSize(), arrS2, mod(rank + 1, size), 1, grid, request2);
    Recv(getMainDomainSize() + 1, getGuardSize() + getMainDomainSize(), mod(rank + 1, size), 0, grid);
    Recv(getGuardSize(), 2 * getGuardSize() - 1, mod(rank - 1, size), 1, grid);

    // ждем отправки
    MPIWrapper::MPIWait(request1);
    MPIWrapper::MPIWait(request2);

    // можно удалить массивы, которые отправляли
    delete[] arrS1;
    delete[] arrS2;
}


void MPIWorker::SendToOneProcess(int dest)
{
    double* arrS;
    MPI_Request request;
    Send(getGuardSize(), getGuardSize() + getMainDomainSize(), arrS, dest, 2, grid, request);
    MPIWrapper::MPIWait(request);
    delete[] arrS;
}

void MPIWorker::RecvFromAllProcesses(Grid3d& gr)
{
    gr = Grid3d(gr.gnxRealCells(), gr.gnyRealCells(), gr.gnzRealCells(), gr.gax(), gr.gbx(), gr.gay(), gr.gby(), gr.gaz(), gr.gbz());

    for (int i = 0; i <= getMainDomainSize(); i++)
        for (int j = 0; j <= gr.gnyRealCells(); j++)
            for (int k = 0; k <= gr.gnzRealCells(); k++)
                gr(i, j, k) = grid(i + getGuardSize(), j, k);

    for (int r = 1; r < MPIWrapper::MPISize(); r++) 
        Recv(r*getMainDomainSize()+1, (r+1)*getMainDomainSize() + 1, r, 2, gr);
}

void MPIWorker::AssembleResultsToZeroProcess(Grid3d& gr)
{
    MPI_Request request;
    if (MPIWrapper::MPIRank() != 0)
        SendToOneProcess(0);
    if (MPIWrapper::MPIRank() == 0)
        RecvFromAllProcesses(gr);
}

void MPIWorker::SetToZerosQuard()
{
    for (int i = 0; i < getGuardSize(); i++)
        for (int j = 0; j < grid.gnyRealNodes(); j++)
            for (int k = 0; k < grid.gnzRealNodes(); k++)
                grid(i, j, k) = Node();

    for (int i = getGuardSize() + getMainDomainSize() + 1; i <= getFullDomainSize(); i++)
        for (int j = 0; j < grid.gnyRealNodes(); j++)
            for (int k = 0; k < grid.gnzRealNodes(); k++)
                grid(i, j, k) = Node();
}

MPIWorker::MPIWorker(Grid3d & gr, int guardWidth, int _size, int _rank)
{
    rank = _rank;
    size = _size;
    domainSize = gr.gnxRealCells() / size - 1; //делится нацело
    domainStart = (domainSize+1)*rank;
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
                    /*if (num % ((n2 - n1 + 1)*grFrom.gnyRealNodes()*grFrom.gnzRealNodes() / 2) == 0)
                        MPIWorker::ShowMessage("pack num=" + std::to_string(num) + " " + std::to_string(i) + " " + std::to_string(j) + " " + std::to_string(k) +
                            " arr[num]=" + std::to_string(grFrom(i, j, k).E[coord]));*/
                }
}

void MPIWorker::UnPackData(int n1, int n2, double *& arr, Grid3d& grTo)
{
    for (int i = n1; i <= n2; i++)
        for (int j = 0; j < grTo.gnyRealNodes(); j++)
            for (int k = 0; k < grTo.gnzRealNodes(); k++)
                for (int coord = 0; coord < 3; coord++) {
                    int num = n*d*((i - n1)*grTo.gnyRealNodes()*grTo.gnzRealNodes() + j*grTo.gnzRealNodes() + k) + n*coord;
                   /* if (num % ((n2 - n1 + 1)*grTo.gnyRealNodes()*grTo.gnzRealNodes() / 2) == 0)
                        MPIWorker::ShowMessage("unpack num=" + std::to_string(num) + " " + std::to_string(i) + " " + std::to_string(j) + " " + std::to_string(k) 
                            + " arr[num]=" + std::to_string(arr[num]));*/
                    grTo(i, j, k).E[coord] += arr[num + 0];
                    grTo(i, j, k).B[coord] += arr[num + 1];
                }
}
