#include "mpi_worker.h"
#include "fourier_transformation.h"
#include "file_writer.h"
#include "operations_with_arrays.h"
#include "class_member_ptr.h"

void MPIWorker::CreateGrid(Grid3d & gr)
{
    vec3<double> a = 0, b = gr.gd()*getFullDomainSize() + a;
    grid = Grid3d(getFullDomainSize(), a, b);
    for (int i = 0; i <= getFullDomainSize().x; i++)
        for (int j = 0; j <= getFullDomainSize().y; j++)
            for (int k = 0; k <= getFullDomainSize().z; k++)
                grid(i, j, k) = gr(mod(vec3<int>(i, j, k) + getMainDomainStart() - getGuardSize(), gr.gnRealCells()));
    ApplyMask();
    FourierTransformation(gr, RtoC);
}

Status MPIWorker::Init(Grid3d & gr, vec3<int> guardWidth, Mask _mask, int _maskWidth) {
    if (checkAndSetParams(gr, guardWidth) == Status::ERROR)
        return Status::ERROR;
    mask = _mask;
    maskWidth = _maskWidth;
    CreateGrid(gr);
    return Status::OK;
}

Status MPIWorker::Initialize(Grid3d & gr, vec3<int> guardWidth, Mask _mask, int _maskWidth, int _size, int _rank) {
    MPIWrapper3d _mpiWrapper3d;
    _mpiWrapper3d.SetSize(_size, 1, 1);
    size = vec3<int>(_size, 1, 1);
    rank = _mpiWrapper3d.getVecRank(_rank);
    mpiWrapper3d = _mpiWrapper3d;
    return Init(gr, guardWidth, _mask, _maskWidth);
}

Status MPIWorker::Initialize(Grid3d & gr, vec3<int> _guardWidth, Mask _mask, int _maskWidth, MPIWrapper3d& _mpiWrapper3d) {
    setMPIWrapper3d(mpiWrapper3d);
    size = mpiWrapper3d.MPISize();
    rank = mpiWrapper3d.MPIRank();
    return Init(gr, _guardWidth, _mask, _maskWidth);
}

Status MPIWorker::Send(vec3<int> n1, vec3<int> n2, double*& arr, vec3<int> dest, int tag, Grid3d& grFrom, MPI_Request& request)
{
    if (n1.x > n2.x || n1.y > n2.y || n1.z > n2.z) return Status::STOP;
    arr = new double[getPackSize(n1, n2)];
    PackData(n1, n2, arr, grFrom);
    mpiWrapper3d.MPIISend(arr, getPackSize(n1, n2), dest, tag, request);
    return Status::OK;
}

Status MPIWorker::Recv(vec3<int> n1, vec3<int> n2, vec3<int> source, int tag, Grid3d& grTo)
{
    if (n1.x > n2.x || n1.y > n2.y || n1.z > n2.z) return Status::ERROR;
    double* arr = new double[getPackSize(n1, n2)];
    mpiWrapper3d.MPIRecv(arr, getPackSize(n1, n2), source, tag);
    UnPackData(n1, n2, arr, grTo);
    if (arr) delete[] arr;
    return Status::OK;
}

template <class T>
vec3<T> getVec(Coordinate coord, T valCoord, vec3<T> val) {
    return vec3<T>(coord == x ? valCoord : val.x, coord == y ? valCoord : val.y, coord == z ? valCoord : val.z);
}

void MPIWorker::ExchangeTwoProcesses(Coordinate coord)
{
    double* arrS1 = 0, *arrS2 = 0;
    MPI_Request request1, request2;

    int sl1, sl2, sr1, sr2, rr1, rr2, rl1, rl2;
    getBoardsForExchange(sl1, sl2, sr1, sr2, rl1, rl2, rr1, rr2, coord);

    vec3<int> prLeft=getVec<int>(coord, mod(rank.*GetCoord<int>(coord) - 1, size.*GetCoord<int>(coord)), rank);
    vec3<int> prRight = getVec<int>(coord, mod(rank.*GetCoord<int>(coord) + 1, size.*GetCoord<int>(coord)), rank);

    //MPIWorker::ShowMessage("send left from " + std::to_string(sl1) + " to " + std::to_string(sl2));
    Send(getVec<int>(coord, sl1, 0), getVec<int>(coord, sl2, grid.gnRealCells()), arrS1, prLeft, 0, grid, request1);
    //MPIWorker::ShowMessage("send right from " + std::to_string(sr1) + " to " + std::to_string(sr2));
    Send(getVec<int>(coord, sr1, 0), getVec<int>(coord, sr2, grid.gnRealCells()), arrS2, prRight, 1, grid, request2);
    //MPIWorker::ShowMessage("recv right from " + std::to_string(rr1) + " to " + std::to_string(rr2));
    Recv(getVec<int>(coord, rr1, 0), getVec<int>(coord, rr2, grid.gnRealCells()), prRight, 0, grid);
    //MPIWorker::ShowMessage("recv left from " + std::to_string(rl1) + " to " + std::to_string(rl2));
    Recv(getVec<int>(coord, rl1, 0), getVec<int>(coord, rl2, grid.gnRealCells()), prLeft, 1, grid);
      
    MPIWrapper::MPIWait(request1);
    MPIWrapper::MPIWait(request2);

    //MPIWorker::ShowMessage("delete arrays");
    if (arrS1) delete[] arrS1;
    if (arrS2) delete[] arrS2;
    if (nameFileAfterExchange != "") {
        //MPIWorker::ShowMessage("writing to file after exchange");
        fileWriter.WriteFile(grid, nameFileAfterExchange);
    }
}

void MPIWorker::getBoardsForExchange(int & sl1, int & sl2, int & sr1, int & sr2,
    int & rl1, int & rl2, int & rr1, int & rr2, Coordinate coord)
{
    sl1 = 0;
    sl2 = 2 * getGuardSize().*GetCoord<int>(coord) - 1;
    sr1 = getMainDomainSize().*GetCoord<int>(coord) + 1;
    sr2 = getFullDomainSize().*GetCoord<int>(coord);
    rr1 = getMainDomainSize().*GetCoord<int>(coord) + 1;
    rr2 = getFullDomainSize().*GetCoord<int>(coord);
    rl1 = 0;
    rl2 = 2 * getGuardSize().*GetCoord<int>(coord) - 1;
}


void MPIWorker::ExchangeGuard()
{
    ExchangeTwoProcesses(x);
    MPIWrapper::MPIBarrier();
    ExchangeTwoProcesses(y);
    MPIWrapper::MPIBarrier();
    ExchangeTwoProcesses(z);
    MPIWrapper::MPIBarrier();
}


void MPIWorker::ApplyMask() {
    for (int i=0; i<=grid.gnxRealCells(); i++)
        for (int j = 0; j <= grid.gnyRealCells(); j++)
            for (int k = 0; k <= grid.gnzRealCells(); k++) {
                grid(i, j, k).E *= mask(vec3<int>(i, j,k), getMainDomainSize(), getGuardSize(), maskWidth);
                grid(i, j, k).B *= mask(vec3<int>(i, j, k), getMainDomainSize(), getGuardSize(), maskWidth);
                grid(i, j, k).J *= mask(vec3<int>(i, j, k), getMainDomainSize(), getGuardSize(), maskWidth);
            }
}

void MPIWorker::SendToOneProcess(vec3<int> dest)
{
    double* arrS=0;
    MPI_Request request;
    Send(getGuardSize(), getGuardSize() + getMainDomainSize(), arrS, dest, 2, grid, request);
    MPIWrapper::MPIWait(request);
    if (arrS) delete[] arrS;
}

void MPIWorker::RecvFromAllProcesses(Grid3d& gr)
{
    gr = Grid3d(gr.gnRealCells(), gr.ga(), gr.gb());

    for (int i = 0; i <= getMainDomainSize().x; i++)
        for (int j = 0; j <= getMainDomainSize().y; j++)
            for (int k = 0; k <= getMainDomainSize().z; k++)
                gr(i, j, k) = grid(vec3<int>(i, j, k) + getGuardSize());

    for (int i = 0; i < size.x; i++)
        for (int j = 0; j < size.y; j++)
            for (int k = 0; k < size.z; k++) {
                if (i ==0 && j ==0 && k == 0) continue;
                vec3<int> r(i, j, k);
                Recv(r*(getMainDomainSize() + 1), (r + 1)*(getMainDomainSize() + 1) - 1, r, 2, gr);
            }
}

void MPIWorker::AssembleResultsToZeroProcess(Grid3d& gr)
{
    if (mpiWrapper3d.MPIRank() != 0)
        SendToOneProcess(0);
    if (mpiWrapper3d.MPIRank() == 0) {
        RecvFromAllProcesses(gr);
        OperationWithArrays::CopyLastNodesFromFirst(gr);
    }
}

int MPIWorker::getPackSize(vec3<int> n1, vec3<int> n2)
{
    return n*d*(n2.x - n1.x + 1)*(n2.y - n1.y + 1)*(n2.z - n1.z + 1);
}

void MPIWorker::PackData(vec3<int> n1, vec3<int> n2, double *& arr, Grid3d& grFrom)
{
    int num = 0;
    for (int i = n1.x; i <= n2.x; i++)
        for (int j = n1.y; j <= n2.y; j++)
            for (int k = n1.z; k <= n2.z; k++)
                for (int coord = 0; coord < 3; coord++) {
                    arr[num++] = grFrom(i, j, k).E[coord];
                    arr[num++] = grFrom(i, j, k).B[coord];
                }
}

void MPIWorker::UnPackData(vec3<int> n1, vec3<int> n2, double *& arr, Grid3d& grTo)
{
    int num = 0;
    for (int i = n1.x; i <= n2.x; i++)
        for (int j = n1.y; j <= n2.y; j++)
            for (int k = n1.z; k <= n2.z; k++)
                for (int coord = 0; coord < 3; coord++) {
                    grTo(i, j, k).E[coord] += arr[num++];
                    grTo(i, j, k).B[coord] += arr[num++];
                }
}
