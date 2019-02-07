#pragma once
#include "mpi_wrapper_3d.h"
#include "grid3d.h"
#include <string>
#include "mask.h"
#include "status.h"

class MPIWorker {
protected:
    MPIWrapper3d mpiWrapper3d;

    const int n = 2, d = 3; //2 вектора (B, E) из 3 компонент
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
    MPIWorker(Grid3d& gr, vec3<int> guardWidth, Mask mask, MPIWrapper3d& _mpiWrapper3d) {
        Initialize(gr, guardWidth, mask, _mpiWrapper3d);
    }

    //для последовательного запуска
    MPIWorker(Grid3d& gr, vec3<int> guardWidth, Mask mask, int _size, int _rank) {
        Initialize(gr, guardWidth, mask, _size, _rank);
    }

    Status Initialize(Grid3d & gr, vec3<int> guardWidth, Mask mask, int _size, int _rank);
	Status Initialize(Grid3d & gr, vec3<int> guardWidth, Mask mask, MPIWrapper3d& _mpiWrapper3d);

    void setMPIWrapper3d(MPIWrapper3d& _mpiWrapper) {
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

    virtual void ApplyMask() = 0;

    void ExchangeGuard();

    static void ShowMessage(std::string message) {
        std::cout << "rank " << MPIWrapper::MPIRank() << ": " << message << std::endl;
    }

    void AssembleResultsToZeroProcess(Grid3d& gr);

protected:
    vec3<int> mod(vec3<int> a, vec3<int> b) {
        return ((a + b) % b);
    }

    int mod(int a, int b) {
        return ((a + b) % b);
    }

    Status Init(Grid3d & gr, vec3<int> guardWidth, Mask _mask);

    //обработка всевозможных ошибок ввода
    Status checkAndSetParams(Grid3d& gr, vec3<int> _guardSize);
    Status checkAndSetGuardSizeAndDomainStart(Grid3d& gr);
    Status setGuardSize(vec3<int> _guardSize);

    void setLeftGuardStart(vec3<int> guardWidth, Grid3d& gr);
    void setRightGuardStart(vec3<int> guardWidth, Grid3d& gr);

    void CreateGrid(Grid3d& gr);

    Status Send(vec3<int> n1, vec3<int> n2, double*& arr, vec3<int> dest, int tag, Grid3d& grFrom, MPI_Request& request);
    Status Recv(vec3<int> n1, vec3<int> n2, vec3<int> source, int tag, Grid3d& grTo);
    void SendToOneProcess(vec3<int> dest);
    void RecvFromAllProcesses(Grid3d& gr);

    //упаковывает вещественные поля части сетки
    int getPackSize(vec3<int> n1, vec3<int> n2);
    void PackData(vec3<int> n1, vec3<int> n2, double *& arr, Grid3d& grFrom);
    virtual void UnPackData(vec3<int> n1, vec3<int> n2, double *& arr, Grid3d& grTo) = 0;

    int getNum(int i, int j, int k) {
        return (i * 3 + j) * 3 + k;
    }

    void ExchangeTwoProcesses(Coordinate coord);
    virtual void getBoardsForExchange(int& sl1, int& sl2, int& sr1, int& sr2,
        int& rl1, int& rl2, int& rr1, int& rr2, Coordinate coord) = 0;
};