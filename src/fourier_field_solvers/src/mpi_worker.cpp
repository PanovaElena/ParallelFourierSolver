#include "mpi_worker.h"
#include "file_writer.h"
#include "class_member_ptr.h"

Status MPIWorker::checkAndSetParams(Grid3d & gr, vec3<int> _guardSize)
{
    if (checkAndSetGuardSizeAndDomainStart(gr) == Status::ERROR)
        return Status::ERROR;
    if (setGuardSize(_guardSize) == Status::ERROR)
        return Status::ERROR;
    setLeftGuardStart(guardSize, gr);
    setRightGuardStart(guardSize, gr);
    return Status::OK;
}

Status MPIWorker::checkAndSetGuardSizeAndDomainStart(Grid3d & gr)
{
    if (gr.gnRealCells().x < size.x || gr.gnRealCells().y < size.y || gr.gnRealCells().z < size.z) {
        ShowMessage("ERROR: domain size is less than MPISize");
        return Status::ERROR;
    }

    domainSize = gr.gnRealCells() / size; //делится нацело
    domainStart = domainSize*rank;

    if (domainSize.x == 0 || domainSize.y == 0 || domainSize.z == 0) {
        ShowMessage("ERROR: domain size x is 0");
        return Status::ERROR;
    }
    return Status::OK;
}

Status MPIWorker::setGuardSize(vec3<int> _guardSize)
{
    guardSize = _guardSize;
    if (domainSize.x <= _guardSize.x) return Status::ERROR;
    if (domainSize.y <= _guardSize.y) return Status::ERROR;
    if (domainSize.z <= _guardSize.z) return Status::ERROR;
    if (domainSize.x == 0 || domainSize.y == 0 || domainSize.z == 0)
        return Status::ERROR;
    return Status::OK;
}

void MPIWorker::setLeftGuardStart(vec3<int> guardWidth, Grid3d & gr)
{
    leftGuardStart = getMainDomainStart() - guardWidth;
    if (rank.x == 0)
        leftGuardStart.x = gr.gnRealCells().x - guardWidth.x;
    if (rank.y == 0)
        leftGuardStart.y = gr.gnRealCells().y - guardWidth.y;
    if (rank.z == 0)
        leftGuardStart.z = gr.gnRealCells().z - guardWidth.z;
}

void MPIWorker::setRightGuardStart(vec3<int> guardWidth, Grid3d & gr)
{
    rightGuardStart = getMainDomainEnd();
    if (rank.x == size.x - 1)
        rightGuardStart.x = 0;
    if (rank.y == size.y - 1)
        rightGuardStart.y = 0;
    if (rank.z == size.z - 1)
        rightGuardStart.z = 0;
}

void MPIWorker::CreateGrid(Grid3d & gr)
{
    vec3<double> a = vec3<>(0), b = gr.gd()*getFullDomainSize() + a;
    grid = Grid3d(getFullDomainSize(), a, b);
    for (int i = 0; i < getFullDomainSize().x; i++)
        for (int j = 0; j < getFullDomainSize().y; j++)
            for (int k = 0; k < getFullDomainSize().z; k++) {
                grid.E.Write(i, j, k, gr.E(mod(vec3<int>(i, j, k) + getMainDomainStart() - getGuardSize(), gr.gnRealCells())));
                grid.B.Write(i, j, k, gr.B(mod(vec3<int>(i, j, k) + getMainDomainStart() - getGuardSize(), gr.gnRealCells())));
                grid.J.Write(i, j, k, gr.J(mod(vec3<int>(i, j, k) + getMainDomainStart() - getGuardSize(), gr.gnRealCells())));
            }
    ApplyMask();
}

Status MPIWorker::Init(Grid3d & gr, vec3<int> guardWidth, Mask _mask) {
    if (checkAndSetParams(gr, guardWidth) == Status::ERROR)
        return Status::ERROR;
    mask = _mask;
    CreateGrid(gr);
    return Status::OK;
}

Status MPIWorker::Initialize(Grid3d & gr, vec3<int> guardWidth, Mask _mask, int _size, int _rank) {
    MPIWrapper3d _mpiWrapper3d;
    _mpiWrapper3d.SetSize(_size, 1, 1);
    size = vec3<int>(_size, 1, 1);
    rank = _mpiWrapper3d.getVecRank(_rank);
    mpiWrapper3d = _mpiWrapper3d;
    return Init(gr, guardWidth, _mask);
}

Status MPIWorker::Initialize(Grid3d & gr, vec3<int> _guardWidth, Mask _mask, MPIWrapper3d& _mpiWrapper3d) {
    setMPIWrapper3d(mpiWrapper3d);
    size = mpiWrapper3d.MPISize();
    rank = mpiWrapper3d.MPIRank();
    return Init(gr, _guardWidth, _mask);
}

Status MPIWorker::Send(vec3<int> n1, vec3<int> n2, double*& arr, vec3<int> dest, 
    int tag, Grid3d& grFrom, MPI_Request& request)
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

void MPIWorker::ExchangeTwoProcesses(Coordinate coord)
{
    if (getGuardSize().*GetCoord<int>(coord) == 0) return;

    double* arrS1 = 0, *arrS2 = 0;
    MPI_Request request1, request2;

    int sl1, sl2, sr1, sr2, rr1, rr2, rl1, rl2;
    getBoardsForExchange(sl1, sl2, sr1, sr2, rl1, rl2, rr1, rr2, coord);

    vec3<int> prLeft= vec3<int>::getVecIfCoord(coord, vec3<int>(mod(rank.*GetCoord<int>(coord) - 1, size.*GetCoord<int>(coord))), rank);
    vec3<int> prRight = vec3<int>::getVecIfCoord(coord, vec3<int>(mod(rank.*GetCoord<int>(coord) + 1, size.*GetCoord<int>(coord))), rank);

    vec3<int> down_left, top_right;

    down_left = vec3<int>::getVecIfCoord(coord, vec3<int>(sl1), vec3<int>(0));
    top_right = vec3<int>::getVecIfCoord(coord, vec3<int>(sl2), grid.gnRealCells());
    //MPIWorker::ShowMessage("send left from " + to_string(down_left) + " to " + to_string(top_right));
    Send(down_left, top_right, arrS1, prLeft, 0, grid, request1);
    
    down_left = vec3<int>::getVecIfCoord(coord, vec3<int>(sr1), vec3<int>(0));
    top_right = vec3<int>::getVecIfCoord(coord, vec3<int>(sr2), grid.gnRealCells());
    //MPIWorker::ShowMessage("send right from " + to_string(down_left) + " to " + to_string(top_right));
    Send(down_left, top_right, arrS2, prRight, 1, grid, request2);
    
    down_left = vec3<int>::getVecIfCoord(coord, vec3<int>(rr1), vec3<int>(0));
    top_right = vec3<int>::getVecIfCoord(coord, vec3<int>(rr2), grid.gnRealCells());
    //MPIWorker::ShowMessage("recv right from " + to_string(down_left) + " to " + to_string(top_right));
    Recv(down_left, top_right, prRight, 0, grid);
    
    down_left = vec3<int>::getVecIfCoord(coord, vec3<int>(rl1), vec3<int>(0));
    top_right = vec3<int>::getVecIfCoord(coord, vec3<int>(rl2), grid.gnRealCells());
    //MPIWorker::ShowMessage("recv left from " + to_string(down_left) + " to " + to_string(top_right));
    Recv(down_left, top_right, prLeft, 1, grid);
      
    MPIWrapper::MPIWait(request1);
    MPIWrapper::MPIWait(request2);

    //MPIWorker::ShowMessage("delete arrays");
    if (arrS1) delete[] arrS1;
    if (arrS2) delete[] arrS2;
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

    for (int i = 0; i < getMainDomainSize().x; i++)
        for (int j = 0; j < getMainDomainSize().y; j++)
            for (int k = 0; k < getMainDomainSize().z; k++) {
                gr.E.Write(i, j, k, grid.E(vec3<int>(i, j, k) + getGuardSize()));
                gr.B.Write(i, j, k, grid.B(vec3<int>(i, j, k) + getGuardSize()));
                gr.J.Write(i, j, k, grid.J(vec3<int>(i, j, k) + getGuardSize()));
            }

    for (int i = 0; i < size.x; i++)
        for (int j = 0; j < size.y; j++)
            for (int k = 0; k < size.z; k++) {
                if (i == 0 && j == 0 && k == 0) continue;
                vec3<int> r(i, j, k);
                Recv(r*getMainDomainSize(), (r + vec3<int>(1))*getMainDomainSize(), r, 2, gr);
            }
}

void MPIWorker::AssembleResultsToZeroProcess(Grid3d& gr)
{
    if (mpiWrapper3d.MPIRank() != vec3<int>(0))
        SendToOneProcess(vec3<int>(0));
    if (mpiWrapper3d.MPIRank() == vec3<int>(0))
        RecvFromAllProcesses(gr);
}

int MPIWorker::getPackSize(vec3<int> n1, vec3<int> n2)
{
    return n*d*(n2.x - n1.x)*(n2.y - n1.y)*(n2.z - n1.z);
}

void MPIWorker::PackData(vec3<int> n1, vec3<int> n2, double *& arr, Grid3d& grFrom)
{
    int num = 0;
    for (int i = n1.x; i < n2.x; i++)
        for (int j = n1.y; j < n2.y; j++)
            for (int k = n1.z; k < n2.z; k++)
                for (int coord = 0; coord < 3; coord++) {
                    arr[num++] = grFrom.E[coord](i, j, k);
                    arr[num++] = grFrom.B[coord](i, j, k);
                }
}
