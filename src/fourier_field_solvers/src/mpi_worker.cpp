#include "mpi_worker.h"
#include "file_writer.h"
#include "class_member_ptr.h"
#include "start_conditions.h"

void MPIWorker::setParams(vec3<int> commonSize, vec3<int> _guardSize) {
    domainSize = commonSize / size;  // ������� ������
    domainStart = domainSize * rank;
    guardSize = _guardSize;
    setLeftGuardStart(guardSize, commonSize);
    setRightGuardStart(guardSize);
}

Status MPIWorker::checkParams(vec3<int> commonSize) {

    if (commonSize % size != vec3<int>(0)) {
        showMessage("ERROR: domain size % MPISize != 0");
        return Status::ERROR;
    }

    if (commonSize.x < size.x || commonSize.y < size.y || commonSize.z < size.z) {
        showMessage("ERROR: domain size is less than MPISize");
        return Status::ERROR;
    }

    if (gr.sizeReal().x / size.x == 0 ||
        gr.sizeReal().y / size.y == 0 ||
        gr.sizeReal().z / size.z == 0) {
        showMessage("ERROR: domain size is 0");
        return Status::ERROR;
    }
    return Status::OK;
}

void MPIWorker::setLeftGuardStart(vec3<int> guardWidth, vec3<int> commonSize) {
    leftGuardStart = getMainDomainStart() - guardWidth;
    if (rank.x == 0)
        leftGuardStart.x = commonSize.x - guardWidth.x;
    if (rank.y == 0)
        leftGuardStart.y = commonSize.y - guardWidth.y;
    if (rank.z == 0)
        leftGuardStart.z = commonSize.z - guardWidth.z;
}

void MPIWorker::setRightGuardStart(vec3<int> guardWidth) {
    rightGuardStart = getMainDomainEnd();
    if (rank.x == size.x - 1)
        rightGuardStart.x = 0;
    if (rank.y == size.y - 1)
        rightGuardStart.y = 0;
    if (rank.z == size.z - 1)
        rightGuardStart.z = 0;
}

void MPIWorker::createGrid(Grid3d & commonGrid) {
    vec3<double> a = (vec3<double>)(getMainDomainStart() - getGuardSize())*commonGrid.getStep() + commonGrid.getStart(),
        b = a + (vec3<double>)getFullDomainSize()*commonGrid.getStep();
    grid = Grid3d(getFullDomainSize(), a, b);
    for (int i = 0; i < getFullDomainSize().x; i++)
        for (int j = 0; j < getFullDomainSize().y; j++)
            for (int k = 0; k < getFullDomainSize().z; k++) {
                grid.E.write(i, j, k, commonGrid.E(mod(vec3<int>(i, j, k) + getMainDomainStart()
                    - getGuardSize(), commonGrid.sizeReal())));
                grid.B.write(i, j, k, commonGrid.B(mod(vec3<int>(i, j, k) + getMainDomainStart()
                    - getGuardSize(), commonGrid.sizeReal())));
                grid.J.write(i, j, k, commonGrid.J(mod(vec3<int>(i, j, k) + getMainDomainStart()
                    - getGuardSize(), commonGrid.sizeReal())));
            }
}

void MPIWorker::createGrid(vec3<int> & _commonSize, StartConditions& sc) {
    vec3<double> a = (vec3<double>)(getMainDomainStart() - getGuardSize())*sc.d + sc.a,
        b = a + (vec3<double>)getFullDomainSize()*sc.d;
    grid = Grid3d(getFullDomainSize(), a, b);
    for (int i = 0; i < getFullDomainSize().x; i++)
        for (int j = 0; j < getFullDomainSize().y; j++)
            for (int k = 0; k < getFullDomainSize().z; k++) {
                grid.E.write(i, j, k, sc.fE(mod(vec3<int>(i, j, k) + getMainDomainStart()
                    - getGuardSize(), _commonSize)));
                grid.B.write(i, j, k, sc.fB(mod(vec3<int>(i, j, k) + getMainDomainStart()
                    - getGuardSize(), _commonSize)));
                grid.J.write(i, j, k, sc.fJ(mod(vec3<int>(i, j, k) + getMainDomainStart()
                    - getGuardSize(), _commonSize), 0));
            }
}

Status MPIWorker::initialize(Grid3d & commonGrid, vec3<int> _guardWidth, Mask _mask, int _size, int _rank) {
    MPIWrapper3d _mpiWrapper3d;
    _mpiWrapper3d.setSize(_size, 1, 1);
    size = vec3<int>(_size, 1, 1);
    rank = _mpiWrapper3d.getVecRank(_rank);
    mpiWrapper3d = _mpiWrapper3d;
    setParams(commonGrid.sizeReal(), _guardWidth);
    if (checkParams(commonGrid.sizeReal()) == Status::ERROR)
        return Status::ERROR;
    mask = _mask;
    createGrid(commonGrid);
    return Status::OK;
}

Status MPIWorker::initialize(Grid3d & commonGrid, vec3<int> _guardWidth, Mask _mask, MPIWrapper3d& _mpiWrapper3d) {
    setMPIWrapper3d(_mpiWrapper3d);
    size = mpiWrapper3d.MPISize();
    rank = mpiWrapper3d.MPIRank();
    setParams(commonGrid.sizeReal(), _guardWidth);
    if (checkParams(commonGrid.sizeReal()) == Status::ERROR)
        return Status::ERROR;
    mask = _mask;
    createGrid(commonGrid);
    return Status::OK;
}

Status MPIWorker::initialize(vec3<int> & _commonSize, vec3<int> _guardWidth, Mask _mask,
    MPIWrapper3d& _mpiWrapper3d, StartConditions& startConditions) {
    setMPIWrapper3d(_mpiWrapper3d);
    size = mpiWrapper3d.MPISize();
    rank = mpiWrapper3d.MPIRank();
    setParams(_commonSize, _guardWidth);
    if (checkParams(_commonSize) == Status::ERROR)
        return Status::ERROR;
    mask = _mask;
    createGrid(_commonSize, startConditions);
    return Status::OK;
}

Status MPIWorker::send(vec3<int> n1, vec3<int> n2, double*& arr, vec3<int> dest,
    int tag, Grid3d& grFrom, MPI_Request& request) {
    if (n1.x > n2.x || n1.y > n2.y || n1.z > n2.z) return Status::STOP;
    arr = new double[getPackSize(n1, n2)];
    packData(n1, n2, arr, grFrom);
    mpiWrapper3d.MPIISend(arr, getPackSize(n1, n2), dest, tag, request);
    return Status::OK;
}

Status MPIWorker::recv(vec3<int> n1, vec3<int> n2, vec3<int> source, int tag, Grid3d& grTo) {
    if (n1.x > n2.x || n1.y > n2.y || n1.z > n2.z) return Status::ERROR;
    double* arr = new double[getPackSize(n1, n2)];
    mpiWrapper3d.MPIRecv(arr, getPackSize(n1, n2), source, tag);
    unpackData(n1, n2, arr, grTo);
    if (arr) delete[] arr;
    return Status::OK;
}

void MPIWorker::exchangeTwoProcesses(Coordinate coord) {
    if (getGuardSize().*getMemberPtrCoord<int>(coord) == 0) return;

    double* arrS1 = 0, *arrS2 = 0;
    MPI_Request request1, request2;

    int sl1, sl2, sr1, sr2, rr1, rr2, rl1, rl2;
    getBoardsForExchange(sl1, sl2, sr1, sr2, rl1, rl2, rr1, rr2, coord);

    vec3<int> prLeft = vec3<int>::getVecIfCoord(coord, vec3<int>(mod(rank.*getMemberPtrCoord<int>(coord) - 1,
        size.*getMemberPtrCoord<int>(coord))), rank);
    vec3<int> prRight = vec3<int>::getVecIfCoord(coord, vec3<int>(mod(rank.*getMemberPtrCoord<int>(coord) + 1,
        size.*getMemberPtrCoord<int>(coord))), rank);

    vec3<int> down_left, top_right;

    down_left = vec3<int>::getVecIfCoord(coord, vec3<int>(sl1), vec3<int>(0));
    top_right = vec3<int>::getVecIfCoord(coord, vec3<int>(sl2), grid.sizeReal());
    //showMessage("send left from " + to_string(down_left) + " to " + to_string(top_right));
    send(down_left, top_right, arrS1, prLeft, 0, grid, request1);

    down_left = vec3<int>::getVecIfCoord(coord, vec3<int>(sr1), vec3<int>(0));
    top_right = vec3<int>::getVecIfCoord(coord, vec3<int>(sr2), grid.sizeReal());
    //showMessage("send right from " + to_string(down_left) + " to " + to_string(top_right));
    send(down_left, top_right, arrS2, prRight, 1, grid, request2);

    down_left = vec3<int>::getVecIfCoord(coord, vec3<int>(rr1), vec3<int>(0));
    top_right = vec3<int>::getVecIfCoord(coord, vec3<int>(rr2), grid.sizeReal());
    //showMessage("recv right from " + to_string(down_left) + " to " + to_string(top_right));
    recv(down_left, top_right, prRight, 0, grid);

    down_left = vec3<int>::getVecIfCoord(coord, vec3<int>(rl1), vec3<int>(0));
    top_right = vec3<int>::getVecIfCoord(coord, vec3<int>(rl2), grid.sizeReal());
    //showMessage("recv left from " + to_string(down_left) + " to " + to_string(top_right));
    recv(down_left, top_right, prLeft, 1, grid);

    MPIWrapper::MPIWait(request1);
    MPIWrapper::MPIWait(request2);

    //showMessage("delete arrays");
    if (arrS1) delete[] arrS1;
    if (arrS2) delete[] arrS2;
}

void MPIWorker::ExchangeGuard() {
    exchangeTwoProcesses(x);
    MPIWrapper::MPIBarrier();
    exchangeTwoProcesses(y);
    MPIWrapper::MPIBarrier();
    exchangeTwoProcesses(z);
    MPIWrapper::MPIBarrier();
}

void MPIWorker::sendToOneProcess(vec3<int> dest) {
    double* arrS = 0;
    MPI_Request request;
    send(getGuardSize(), getGuardSize() + getMainDomainSize(), arrS, dest, 2, grid, request);
    MPIWrapper::MPIWait(request);
    if (arrS) delete[] arrS;
}

void MPIWorker::recvFromAllProcesses(Grid3d& commonGrid) {
    commonGrid = Grid3d(commonGrid.sizeReal(), commonGrid.getStart(), commonGrid.getEnd());

    for (int i = 0; i < getMainDomainSize().x; i++)
        for (int j = 0; j < getMainDomainSize().y; j++)
            for (int k = 0; k < getMainDomainSize().z; k++) {
                commonGrid.E.write(i, j, k, grid.E(vec3<int>(i, j, k) + getGuardSize()));
                commonGrid.B.write(i, j, k, grid.B(vec3<int>(i, j, k) + getGuardSize()));
                commonGrid.J.write(i, j, k, grid.J(vec3<int>(i, j, k) + getGuardSize()));
            }

    for (int i = 0; i < size.x; i++)
        for (int j = 0; j < size.y; j++)
            for (int k = 0; k < size.z; k++) {
                if (i == 0 && j == 0 && k == 0) continue;
                vec3<int> r(i, j, k);
                recv(r*getMainDomainSize(), (r + vec3<int>(1))*getMainDomainSize(), r, 2, commonGrid);
            }
}

void MPIWorker::assembleResultsToZeroProcess(Grid3d& commonGrid) {
    if (mpiWrapper3d.MPIRank() != vec3<int>(0))
        sendToOneProcess(vec3<int>(0));
    if (mpiWrapper3d.MPIRank() == vec3<int>(0))
        recvFromAllProcesses(commonGrid);
}

int MPIWorker::getPackSize(vec3<int> n1, vec3<int> n2) {
    return N_FIELD * N_DIM * (n2.x - n1.x) * (n2.y - n1.y) * (n2.z - n1.z);
}

void MPIWorker::packData(vec3<int> n1, vec3<int> n2, double *& arr, Grid3d& grFrom) {
    int num = 0;
    for (int i = n1.x; i < n2.x; i++)
        for (int j = n1.y; j < n2.y; j++)
            for (int k = n1.z; k < n2.z; k++)
                for (int coord = 0; coord < 3; coord++) {
                    arr[num++] = grFrom.E[coord](i, j, k);
                    arr[num++] = grFrom.B[coord](i, j, k);
                }
}
