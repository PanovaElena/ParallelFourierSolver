#pragma once
#include "mpi_wrapper.h"
#include "grid3d.h"

class MPIWorker {
private:
    int rank;
    int size;
    int domainSize;
    int guardSize;
    int domainStart;
    int leftGuardStart;
    int rightGuardStart;

    Grid3d grid;
public:
    MPIWorker(Grid3d& gr, double guardWidthPerc/*устанавливает ширину quard в процентах от ширины домена*/) 
        : MPIWorker(gr, guardWidthPerc, MPIWrapper::MPISize(), MPIWrapper::MPIRank()) {}

    //для последовательного запуска
    MPIWorker(Grid3d& gr, double guardWidthPerc/*устанавливает ширину quard в процентах от ширины домена*/, int _size, int _rank);

    int getMainDomainStart() {
        return domainStart;
    }
    int getFullDomainStart() {
        return leftGuardStart;
    }
    int getMainDomainEnd() {
        return domainStart + domainSize;
    }
    int getFullDomainEnd() {
        return rightGuardStart + guardSize;
    }
    int getLeftGuardStart() {
        return leftGuardStart;
    }
    int getLeftGuardEnd() {
        return leftGuardStart + guardSize;
    }
    int getRightGuardStart() {
        return rightGuardStart;
    }
    int getRightGuardEnd() {
        rightGuardStart + guardSize;
    }
    int getFullDomainSize() {
        return 2 * guardSize + domainSize;
    }
    int getMainDomainSize() {
        return domainSize;
    }
    int getGuardSize() {
        return guardSize;
    }
    int getSize() { 
        return size; 
    }
    int getRank() {
        return rank;
    }

    void SetToZerosQuard();

    void ExchangeGuard();

private:
    int mod(int a, int b) {
        return ((a + b) % b);
    }

    void setLeftGuardStart(int guardWidth, Grid3d& gr);

    void setRightGuardStart(int guardWidth, Grid3d& gr);

    void CreateGrid(Grid3d& gr);

    void SendGuard(double*& arr, int& size, int n1, int n2, int dest);
    void RecvGuard(double*& arr, int& size, int n1, int n2, int source);

    //упаковывает вещественные поля части сетки
    void PackData(Grid3d& gr, int n1, int n2, double*& arr, int& size);
    void UnPackData(Grid3d& gr, int n1, int n2, double* arr);
};