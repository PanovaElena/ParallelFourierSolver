#pragma once
#include "mpi_wrapper.h"
#include "grid3d.h"
#include <string>
#include "file_writer.h"

class MPIWorker {
protected:
    const int n = 2, d = 3; //2 ������� (B, E) �� 3 ���������
    int rank;
    int size;
    int domainSize;
    int guardSize;
    int domainStart;
    int leftGuardStart;
    int rightGuardStart;

    Grid3d grid;

    std::string nameFileAfterExchange;
    FileWriter fileWriter;

public:
    MPIWorker() {}
    MPIWorker(Grid3d& gr, int guardWidth) {
        Initialize(gr, guardWidth);
    }

    //��� ����������������� �������
    MPIWorker(Grid3d& gr, int guardWidth, int _size, int _rank) {
        Initialize(gr, guardWidth, _size, _rank);
    }

    void Initialize(Grid3d & gr, int guardWidth, int _size, int _rank);
    void Initialize(Grid3d & gr, int guardWidth);

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

    void SetToZerosQuard();

    void ExchangeGuard();

    static void ShowMessage(std::string message) {
        std::cout << "rank " << MPIWrapper::MPIRank() << ": " << message << std::endl;
    }

    // gr - ����� ��� �����������, ����� ������ ���������� (rank==0), ��� ����� ������ (rank!=0) 
    void AssembleResultsToZeroProcess(Grid3d& gr);

    virtual void DoAfterSeparation() {}

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

    void Send(int n1, int n2, double*& arr, int dest, int tag, Grid3d& grFrom, MPI_Request& request);
    void Recv(int n1, int n2, int source, int tag, Grid3d& grTo);
    void SendToOneProcess(int dest);
    void RecvFromAllProcesses(Grid3d& gr);

    //����������� ������������ ���� ����� �����
    int getPackSize(int n1, int n2);
    void PackData(int n1, int n2, double *& arr, Grid3d& grFrom);
    void UnPackData(int n1, int n2, double *& arr, Grid3d& grTo);
};