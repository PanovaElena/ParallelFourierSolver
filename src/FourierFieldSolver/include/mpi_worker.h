#pragma once
#include "mpi_wrapper.h"
#include "grid3d.h"
#include <string>
#include "file_writer.h"
#include "masks.h"

class MPIWorker {
protected:
    const int n = 2, d = 3; //2 вектора (B, E) из 3 компонент
    int rank;
    int size;
    int domainSize;
    int guardSize;
    int domainStart;
    int leftGuardStart;
    int rightGuardStart;

    Mask mask;
    int maskWidth;

    Grid3d grid;

    std::string nameFileAfterExchange;
    FileWriter fileWriter;

public:
    MPIWorker() {}
    MPIWorker(Grid3d& gr, int guardWidth, Mask mask, int maskWidth) {
        Initialize(gr, guardWidth, mask, maskWidth);
    }

    //для последовательного запуска
    MPIWorker(Grid3d& gr, int guardWidth, Mask mask, int maskWidth, int _size, int _rank) {
        Initialize(gr, guardWidth, mask, maskWidth, _size, _rank);
    }

    void Initialize(Grid3d & gr, int guardWidth, Mask mask, int maskWidth, int _size, int _rank);
    void Initialize(Grid3d & gr, int guardWidth, Mask mask, int maskWidth);

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
        return rightGuardStart + guardSize;
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
    Grid3d& getGrid() {
        return grid;
    }

    void ApplyMask();

    void ExchangeGuard();

    static void ShowMessage(std::string message) {
        std::cout << "rank " << MPIWrapper::MPIRank() << ": " << message << std::endl;
    }

    void AssembleResultsToZeroProcess(Grid3d& gr);

    void SetOutput(FileWriter& _fileWriter, std::string _nameFileAfterExchange) {
        fileWriter = _fileWriter;
        nameFileAfterExchange = _nameFileAfterExchange;
    }

private:
    int mod(int a, int b) {
        return ((a + b) % b);
    }

    void setLeftGuardStart(int guardWidth, Grid3d& gr);

    void setRightGuardStart(int guardWidth, Grid3d& gr);

    void CreateGrid(Grid3d& gr);

    void SetToZerosQuard();

    void Send(int n1, int n2, double*& arr, int dest, int tag, Grid3d& grFrom, MPI_Request& request);
    void Recv(int n1, int n2, int source, int tag, Grid3d& grTo);
    void SendToOneProcess(int dest);
    void RecvFromAllProcesses(Grid3d& gr);

    //упаковывает вещественные поля части сетки
    int getPackSize(int n1, int n2);
    void PackData(int n1, int n2, double *& arr, Grid3d& grFrom);
    void UnPackData(int n1, int n2, double *& arr, Grid3d& grTo);
};