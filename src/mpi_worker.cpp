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
    FourierTransformation(gr, RtoC);
}

void MPIWorker::SendGuard(int n1, int dest, int tag)
{
    double* arr = new double[getPackSize()];
    MPIWorker::ShowMessage("parallel: exchange: send: pack");
    PackData(n1, arr);
    MPIWorker::ShowMessage("parallel: exchange: send: start MPI send to "+std::to_string(dest));
    MPIWrapper::MPISend(arr, getPackSize(), dest, tag);
    MPIWorker::ShowMessage("parallel: exchange: send: end MPI send to " + std::to_string(dest));
    delete[] arr;
}

void MPIWorker::RecvGuard(int n1, int source, int tag)
{
    double* arr = new double[getPackSize()];
    MPIWrapper::MPIRecv(arr, getPackSize(), source, tag);
    UnPackData(n1, arr);
    delete[] arr;
}

void MPIWorker::ExchangeGuard()
{
    MPIWorker::ShowMessage("parallel: exchange: send left");
    SendGuard(0, mod(rank - 1, size), 0);
    MPIWorker::ShowMessage("parallel: exchange: send right df="+std::to_string(getFullDomainSize()));
    SendGuard(getGuardSize() + getMainDomainSize(), mod(rank + 1, size), 1);
    MPIWorker::ShowMessage("parallel: exchange: recv right");
    RecvGuard(getGuardSize() + getMainDomainSize(), mod(rank + 1, size), 0);
    MPIWorker::ShowMessage("parallel: exchange: recv left");
    RecvGuard(0, mod(rank - 1, size), 1);
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

int MPIWorker::getPackSize()
{
    return n*d*(getGuardSize()+1)*grid.gnyRealNodes()*grid.gnzRealNodes();
}

void MPIWorker::PackData(int n1, double *& arr)
{
    int n2 = n1 + getGuardSize();
    MPIWorker::ShowMessage("exchange: send: pack: n1 n2 " + std::to_string(n1) + " " + std::to_string(n2));
    for (int i = n1; i <= n2; i++)
        for (int j = 0; j < grid.gnyRealNodes(); j++)
            for (int k = 0; k < grid.gnzRealNodes(); k++)
                for (int coord = 0; coord < 2; coord++) {
                    int num = n*d*(i*grid.gnyRealNodes()*grid.gnzRealNodes() + j*grid.gnzRealNodes() + k) + n*coord;
                    try {
                        arr[num + 0] = grid(i, j, k).E[coord];
                        arr[num + 1] = grid(i, j, k).B[coord];
                    }
                    catch(std::exception e) {
                        MPIWorker::ShowMessage("exchange: send: pack: fail");
                        std::exit(1);
                    }
                }
    MPIWorker::ShowMessage("exchange: send: pack: done");
}

void MPIWorker::UnPackData(int n1, double *& arr)
{
    int n2 = n1 + getGuardSize();
    for (int i = n1; i <= n2; i++)
        for (int j = 0; j < grid.gnyRealNodes(); j++)
            for (int k = 0; k < grid.gnzRealNodes(); k++)
                for (int coord = 0; coord < 2; coord++) {
                    int num = n*d*(i*grid.gnyRealNodes()*grid.gnzRealNodes() + j*grid.gnzRealNodes() + k) + n*coord;
                    grid(i, j, k).E[coord] += arr[num + 0];
                    grid(i, j, k).B[coord] += arr[num + 1];
                }
}
