#pragma once
#include "mpi_wrapper_3d.h"
#include <string>
#include "grid3d.h"
#include "mask.h"
#include "status.h"
#include "start_conditions.h"

class MPIWorker {
protected:
    MPIWrapper3d mpiWrapper3d;

    const int N_FIELD = 2, N_DIM = 3; //2 вектора (B, E) из 3 компонент
    vec3<int> rank;
    vec3<int> size;
    vec3<int> domainSize;
    vec3<int> guardSize;
    vec3<int> domainStart;
    vec3<int> leftGuardStart;
    vec3<int> rightGuardStart;

    Mask mask;
    vec3<int> maskWidth;

    Grid3d grid;

    std::string nameFileAfterExchange;

public:
    MPIWorker() {}

    // хранение общей сетки на нулевом процессе
    MPIWorker(Grid3d& commonGrid, vec3<int> guardWidth, const Mask& mask, const MPIWrapper3d& _mpiWrapper3d) {
        initialize(commonGrid, guardWidth, mask, _mpiWrapper3d);
    }

    // без общей сетки на нулевом процессе
    MPIWorker(vec3<int> commonSize, vec3<int> guardWidth, const Mask& mask,
        const MPIWrapper3d& _mpiWrapper3d, const StartConditions& startConditions) {
        initialize(commonSize, guardWidth, mask, _mpiWrapper3d, startConditions);
    }

    // для последовательного запуска (тесты)
    MPIWorker(Grid3d& commonGrid, vec3<int> guardWidth, const Mask& mask, int _size, int _rank) {
        initialize(commonGrid, guardWidth, mask, _size, _rank);
    }

    Status initialize(Grid3d & commonGrid, vec3<int> _guardWidth, const Mask& _mask, int _size, int _rank);
    Status initialize(Grid3d & commonGrid, vec3<int> _guardWidth, const Mask& _mask, const MPIWrapper3d& _mpiWrapper3d);
    Status initialize(vec3<int> _commonSize, vec3<int> _guardWidth, const Mask& _mask,
        const MPIWrapper3d& _mpiWrapper3d, const StartConditions& startConditions);

    void setMPIWrapper3d(const MPIWrapper3d& _mpiWrapper) {
        mpiWrapper3d = _mpiWrapper;
    }

    MPIWrapper3d& getMPIWrapper() {
        return mpiWrapper3d;
    }

    vec3<int> getMainDomainStart() {
        return domainStart;
    }
    vec3<int> getFullDomainStart() {
        return leftGuardStart;
    }
    vec3<int> getMainDomainEnd() {
        return domainStart + domainSize;
    }
    vec3<int> getFullDomainEnd() {
        return rightGuardStart + guardSize;
    }
    vec3<int> getLeftGuardStart() {
        return leftGuardStart;
    }
    vec3<int> getLeftGuardEnd() {
        return leftGuardStart + guardSize;
    }
    vec3<int> getRightGuardStart() {
        return rightGuardStart;
    }
    vec3<int> getRightGuardEnd() {
        return rightGuardStart + guardSize;
    }
    vec3<int> getFullDomainSize() {
        return 2 * guardSize + domainSize;
    }
    vec3<int> getMainDomainSize() {
        return domainSize;
    }
    vec3<int> getGuardSize() {
        return guardSize;
    }
    vec3<int> getSize() {
        return size;
    }
    vec3<int> getRank() {
        return rank;
    }
    Grid3d& getGrid() {
        return grid;
    }

    virtual void applyMask() {};

    void ExchangeGuard();

    void showMessage(std::string message) {
        std::cout << "rank " << mpiWrapper3d.getScalarRank(rank) << ": " << message << std::endl;
    }

    void assembleResultsToZeroProcess(Grid3d& commonGrid);

protected:
    vec3<int> mod(vec3<int> a, vec3<int> b) {
        return ((a + b) % b);
    }

    int mod(int a, int b) {
        return ((a + b) % b);
    }

    //обработка всевозможных ошибок ввода
    void setParams(vec3<int> commonSize, vec3<int> _guardSize);
    Status checkParams(vec3<int> commonSize);

    void setLeftGuardStart(vec3<int> guardWidth, vec3<int> commonSize);
    void setRightGuardStart(vec3<int> guardWidth);

    void createGrid(Grid3d& commonGrid);
    void MPIWorker::createGrid(vec3<int> _commonSize, const StartConditions& _startConditions);

    Status send(vec3<int> n1, vec3<int> n2, double*& arr, vec3<int> dest, int tag, const Grid3d& grFrom,
        MPI_Request& request);
    Status recv(vec3<int> n1, vec3<int> n2, vec3<int> source, int tag, Grid3d& grTo);
    void sendToOneProcess(vec3<int> dest);
    void recvFromAllProcesses(Grid3d& commonGrid);

    //упаковывает вещественные поля части сетки
    int getPackSize(vec3<int> n1, vec3<int> n2);
    void packData(vec3<int> n1, vec3<int> n2, double *& arr, const Grid3d& grFrom);
    virtual void unpackData(vec3<int> n1, vec3<int> n2, double *& arr, Grid3d& grTo) {};

    int getNum(int i, int j, int k) {
        return (i * 3 + j) * 3 + k;
    }

    void exchangeTwoProcesses(Coordinate coord);
    virtual void getBoardsForExchange(int& sl1, int& sl2, int& sr1, int& sr2,
        int& rl1, int& rl2, int& rr1, int& rr2, Coordinate coord) {};
};