#include "mpi_worker.h"
#include "fourier_transformation.h"
#include "file_writer.h"
#include "operations_with_arrays.h"

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

int MPIWorker::Init(Grid3d & gr, vec3<int> guardWidth, Mask _mask, int _maskWidth) {
    if (checkAndSetParams(gr, guardWidth) == 1)
        return 1;
    mask = _mask;
    maskWidth = _maskWidth;
    CreateGrid(gr);
    return 0;
}

int MPIWorker::Initialize(Grid3d & gr, vec3<int> guardWidth, Mask _mask, int _maskWidth, int _size, int _rank) {
    MPIWrapper3d _mpiWrapper3d;
    _mpiWrapper3d.SetSize(_size, 1, 1);
    size = vec3<int>(_size, 1, 1);
    rank = _mpiWrapper3d.getVecRank(_rank);
    mpiWrapper3d = _mpiWrapper3d;
    return Init(gr, guardWidth, _mask, _maskWidth);
}

int MPIWorker::Initialize(Grid3d & gr, vec3<int> _guardWidth, Mask _mask, int _maskWidth, MPIWrapper3d& _mpiWrapper3d) {
    setMPIWrapper3d(mpiWrapper3d);
    size = mpiWrapper3d.MPISize();
    rank = mpiWrapper3d.MPIRank();
    return Init(gr, _guardWidth, _mask, _maskWidth);
}

int MPIWorker::Send(vec3<int> n1, vec3<int> n2, double*& arr, vec3<int> dest, int tag, Grid3d& grFrom, MPI_Request& request)
{
    if (n1.x > n2.x || n1.y > n2.y || n1.z > n2.z) return 1;
    arr = new double[getPackSize(n1, n2)];
    PackData(n1, n2, arr, grFrom);
    mpiWrapper3d.MPIISend(arr, getPackSize(n1, n2), dest, tag, request);
    return 0;
}

int MPIWorker::Recv(vec3<int> n1, vec3<int> n2, vec3<int> source, int tag, Grid3d& grTo)
{
    if (n1.x > n2.x || n1.y > n2.y || n1.z > n2.z) return 1;
    double* arr = new double[getPackSize(n1, n2)];
    mpiWrapper3d.MPIRecv(arr, getPackSize(n1, n2), source, tag);
    UnPackData(n1, n2, arr, grTo);
    if (arr) delete[] arr;
    return 0;
}

// неблокирующий send
void MPIWorker::ExchangeSend(double** arrS, MPI_Request* request) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++) {
                if (i == 1 && j == 1 && k == 1) continue;
                else {
                    vec3<int> n1 = getN1Send(i, j, k);
                    vec3<int> n2 = getN2Send(i, j, k);
                    vec3<int> dim = getDim(i, j, k);
                    vec3<int> dest = mod(rank + dim, size);
                    if (dest != rank) {
                        Send(n1, n2, arrS[getNum(i, j, k)], dest, mpiWrapper3d.getTag(dim), grid, request[getNum(i, j, k)]);
                            /*MPIWorker::ShowMessage("rank=" + to_string(rank) + " send to " + to_string(dest) +
                                " from " + to_string(n1) + " to " + to_string(n2) + " tag is " +
                                std::to_string(mpiWrapper3d.getTag(dim)));*/
                    }
                }
            }
}

// блокирующий recv
void MPIWorker::ExchangeRecv() {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++) {
                if (i == 1 && j == 1 && k == 1) continue;
                else {
                    vec3<int> n1 = getN1Recv(i, j, k);
                    vec3<int> n2 = getN2Recv(i, j, k);
                    vec3<int> dim = getDim(i, j, k);
                    vec3<int> source = mod(rank + dim, size);
                    if (source != rank) {
                        Recv(n1, n2, source, mpiWrapper3d.getTag(-1 * dim), grid);
                            /*MPIWorker::ShowMessage("rank=" + to_string(rank) + " recv from " + to_string(source) +
                                " from " + to_string(n1) + " to " + to_string(n2) + " tag is " +
                                std::to_string(mpiWrapper3d.getTag(-1 * dim)));*/
                    }
                }
            }
}

// ждем отправки
void MPIWorker::ExchangeWait(MPI_Request* request) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++) {
                if (i == 1 && j == 1 && k == 1) continue;
                else if (request[getNum(i, j, k)] != 0)
                    MPIWrapper::MPIWait(request[getNum(i, j, k)]);
            }
}

void MPIWorker::ExchangeGuard()
{
    const int N = 3 * 3 * 3;
    double* arrS[N] = { 0 };
    MPI_Request request[N] = { 0 };

    ExchangeSend(arrS, request);
    ExchangeRecv();
    ExchangeWait(request);

    // можно удалить массивы, которые отправляли
    //MPIWorker::ShowMessage("delete arrays");
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++)
                if (arrS[getNum(i, j, k)]) delete[] arrS[getNum(i, j, k)];

    if (nameFileAfterExchange != "") {
        //MPIWorker::ShowMessage("writing to file after exchange");
        fileWriter.WriteFile(grid, nameFileAfterExchange);
    }
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
