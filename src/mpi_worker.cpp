#include "mpi_worker.h"
#include "fourier_transformation.h"
#include "write_file.h"
#include "operations_with_arrays.h"

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
    grid = Grid3d(getFullDomainSize(), gr.gnyRealCells(), gr.gnzRealCells(), a, b, gr.gay(), gr.gby(), gr.gaz(), gr.gbz());
    for (int i = 0; i <= getFullDomainSize(); i++)
        for (int j = 0; j < gr.gnyRealNodes(); j++)
            for (int k = 0; k < gr.gnzRealNodes(); k++)
                grid(i, j, k) = gr(mod(i + getLeftGuardStart(), gr.gnxRealCells()), j, k);
    SetToZerosQuard();
    FourierTransformation(gr, RtoC);
}

void MPIWorker::Initialize(Grid3d & gr, int guardWidth, int _size, int _rank) {
    rank = _rank;
    size = _size;
    domainSize = gr.gnxRealCells() / size - 1; //делится нацело
    domainStart = (domainSize + 1)*rank;
    guardSize = guardWidth;
    setLeftGuardStart(guardSize, gr);
    setRightGuardStart(guardSize, gr);

    CreateGrid(gr);
    DoAfterSeparation();
}

void MPIWorker::Initialize(Grid3d & gr, int guardWidth) {
    Initialize(gr, guardWidth, MPIWrapper::MPISize(), MPIWrapper::MPIRank());
}

void MPIWorker::Send(int n1, int n2, double*& arr, int dest, int tag, Grid3d& grFrom, MPI_Request& request)
{
    arr = new double[getPackSize(n1, n2)];
    PackData(n1, n2, arr, grFrom);
    MPIWrapper::MPIISend(arr, getPackSize(n1, n2), dest, tag, request);
}

void MPIWorker::Recv(int n1, int n2, int source, int tag, Grid3d& grTo)
{
    double* arr = new double[getPackSize(n1, n2)];
    MPIWrapper::MPIRecv(arr, getPackSize(n1, n2), source, tag);
    UnPackData(n1, n2, arr, grTo);
    if (arr) delete[] arr;
}

void MPIWorker::ExchangeGuard()
{
    double* arrS1 = 0, *arrS2 = 0;
    MPI_Request request1, request2;

    int sl1 = 0, sl2 = getGuardSize() - 1;
    int sr1 = getGuardSize() + getMainDomainSize() + 1, sr2 = getFullDomainSize();
    int rr1 = getMainDomainSize() + 1, rr2 = getGuardSize() + getMainDomainSize();
    int rl1 = getGuardSize(), rl2 = 2 * getGuardSize() - 1;

    // неблокирующий send и блокирующий recv
    MPIWorker::ShowMessage("send left from " + std::to_string(sl1) + " to " + std::to_string(sl2));
    Send(sl1, sl2, arrS1, mod(rank - 1, size), 0, grid, request1);

    MPIWorker::ShowMessage("send right from " + std::to_string(sr1) + " to " + std::to_string(sr2));
    Send(sr1, sr2, arrS2, mod(rank + 1, size), 1, grid, request2);

    MPIWorker::ShowMessage("recv right from " + std::to_string(rr1) + " to " + std::to_string(rr2));
    Recv(rr1, rr2, mod(rank + 1, size), 0, grid);

    MPIWorker::ShowMessage("recv left from " + std::to_string(rl1) + " to " + std::to_string(rl2));
    Recv(rl1, rl2, mod(rank - 1, size), 1, grid);

    // ждем отправки
    MPIWrapper::MPIWait(request1);
    MPIWrapper::MPIWait(request2);

    // можно удалить массивы, которые отправляли
    MPIWorker::ShowMessage("delete arrays");
    if (arrS1) delete[] arrS1;
    if (arrS2) delete[] arrS2;

    if (nameFileAfterExchange != "") {
        MPIWorker::ShowMessage("writing to file after exchange");
        WriteFileField2d(E, y, grid, nameFileAfterExchange);
    }
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

void MPIWorker::SendToOneProcess(int dest)
{
    double* arrS;
    MPI_Request request;
    Send(getGuardSize(), getGuardSize() + getMainDomainSize(), arrS, dest, 2, grid, request);
    MPIWrapper::MPIWait(request);
    if (arrS) delete[] arrS;
}

void MPIWorker::RecvFromAllProcesses(Grid3d& gr)
{
    gr = Grid3d(gr.gnxRealCells(), gr.gnyRealCells(), gr.gnzRealCells(), 
        gr.gax(), gr.gbx(), gr.gay(), gr.gby(), gr.gaz(), gr.gbz());

    for (int i = 0; i <= getMainDomainSize(); i++)
        for (int j = 0; j <= gr.gnyRealCells(); j++)
            for (int k = 0; k <= gr.gnzRealCells(); k++)
                gr(i, j, k) = grid(i + getGuardSize(), j, k);

    for (int r = 1; r < MPIWrapper::MPISize(); r++) 
        Recv(r*(getMainDomainSize()+1), (r+1)*(getMainDomainSize()+1) - 1, r, 2, gr);
}

void MPIWorker::AssembleResultsToZeroProcess(Grid3d& gr)
{
    if (MPIWrapper::MPIRank() != 0)
        SendToOneProcess(0);
    if (MPIWrapper::MPIRank() == 0) {
        RecvFromAllProcesses(gr);
        OperationWithArrays::CopyLastNodesFromFirst(gr);
    }
}

int MPIWorker::getPackSize(int n1, int n2)
{
    return n*d*(n2 - n1 + 1)*grid.gnyRealNodes()*grid.gnzRealNodes();
}

void MPIWorker::PackData(int n1, int n2, double *& arr, Grid3d& grFrom)
{
    int num = 0;
    for (int i = n1; i <= n2; i++)
        for (int j = 0; j < grFrom.gnyRealNodes(); j++)
            for (int k = 0; k < grFrom.gnzRealNodes(); k++)
                for (int coord = 0; coord < 3; coord++) {
                    arr[num++] = grFrom(i, j, k).E[coord];
                    arr[num++] = grFrom(i, j, k).B[coord];
                }
}

void MPIWorker::UnPackData(int n1, int n2, double *& arr, Grid3d& grTo)
{
    int num = 0;
    for (int i = n1; i <= n2; i++)
        for (int j = 0; j < grTo.gnyRealNodes(); j++)
            for (int k = 0; k < grTo.gnzRealNodes(); k++)
                for (int coord = 0; coord < 3; coord++) { 
                    grTo(i, j, k).E[coord] += arr[num++];
                    grTo(i, j, k).B[coord] += arr[num++];
                }
}
