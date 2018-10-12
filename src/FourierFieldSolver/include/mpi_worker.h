#pragma once
#include "mpi_wrapper_3d.h"
#include "grid3d.h"
#include <string>
#include "file_writer.h"
#include "masks.h"
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
    int maskWidth;

    Grid3d grid;

    std::string nameFileAfterExchange;
    FileWriter fileWriter;

public:
    MPIWorker() {}
    MPIWorker(Grid3d& gr, vec3<int> guardWidth, Mask mask, int maskWidth, MPIWrapper3d& _mpiWrapper3d) {
        Initialize(gr, guardWidth, mask, maskWidth, _mpiWrapper3d);
    }

    //для последовательного запуска
    MPIWorker(Grid3d& gr, vec3<int> guardWidth, Mask mask, int maskWidth, int _size, int _rank) {
        Initialize(gr, guardWidth, mask, maskWidth, _size, _rank);
    }

    Status Initialize(Grid3d & gr, vec3<int> guardWidth, Mask mask, int maskWidth, int _size, int _rank);
	Status Initialize(Grid3d & gr, vec3<int> guardWidth, Mask mask, int maskWidth, MPIWrapper3d& _mpiWrapper3d);

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
    vec3<int> mod(vec3<int> a, vec3<int> b) {
        return ((a + b) % b);
    }

    Status Init(Grid3d & gr, vec3<int> guardWidth, Mask _mask, int _maskWidth);

    //чтобы не было доменов нулевого размера
	Status checkAndSetParams(Grid3d& gr, vec3<int> _guardSize) {
        if (checkAndSetGuardSizeAndDomainStart(gr) == Status::ERROR)
            return Status::ERROR;
        if (setGuardSize(_guardSize)==Status::ERROR)
			return Status::ERROR;
        setLeftGuardStart(guardSize, gr);
        setRightGuardStart(guardSize, gr);
        return Status::OK;
    }
	Status checkAndSetGuardSizeAndDomainStart(Grid3d& gr) {
        if (gr.gnRealCells().x < size.x || gr.gnRealCells().y < size.y || gr.gnRealCells().z < size.z) {
            ShowMessage("ERROR: domain size is less than MPISize");
            return Status::ERROR;
        }
        domainSize = gr.gnRealCells() / size - 1; //делится нацело
        domainStart = (domainSize + 1)*rank;
        if (domainSize.x == 0) {
            if (size.x == 1)
                domainSize.x = 1;
            else {
                ShowMessage("ERROR: domain size x is 0");
                return Status::ERROR;
            }
        }
        if (domainSize.y == 0) {
            if (size.y == 1)
                domainSize.y = 1;
            else {
                ShowMessage("ERROR: domain size y is 0");
                return Status::ERROR;
            }
        }
        if (domainSize.z == 0) {
            if (size.z == 1)
                domainSize.z = 1;
            else {
                ShowMessage("ERROR: domain size z is 0");
                return Status::ERROR;
            }
        }
        return Status::OK;
    }
    Status setGuardSize(vec3<int> _guardSize) {
        guardSize = _guardSize;
        if (domainSize.x <= _guardSize.x) guardSize.x = domainSize.x - 1;
        if (domainSize.y <= _guardSize.y) guardSize.y = domainSize.y - 1;
        if (domainSize.z <= _guardSize.z) guardSize.z = domainSize.z - 1;
		if (domainSize.x == 0 || domainSize.y == 0 || domainSize.z == 0)
			return Status::ERROR;
		return Status::OK;
    }
    void setLeftGuardStart(vec3<int> guardWidth, Grid3d& gr) {
        leftGuardStart = getMainDomainStart() - guardWidth;
        if (rank.x == 0)
            leftGuardStart.x = gr.gnRealCells().x - guardWidth.x;
        if (rank.y == 0)
            leftGuardStart.y = gr.gnRealCells().y - guardWidth.y;
        if (rank.z == 0)
            leftGuardStart.z = gr.gnRealCells().z - guardWidth.z;
    }
    void setRightGuardStart(vec3<int> guardWidth, Grid3d& gr) {
        rightGuardStart = getMainDomainEnd();
    }

    void CreateGrid(Grid3d& gr);

    Status Send(vec3<int> n1, vec3<int> n2, double*& arr, vec3<int> dest, int tag, Grid3d& grFrom, MPI_Request& request);
    Status Recv(vec3<int> n1, vec3<int> n2, vec3<int> source, int tag, Grid3d& grTo);
    void SendToOneProcess(vec3<int> dest);
    void RecvFromAllProcesses(Grid3d& gr);

    void ExchangeSend(double** arrS, MPI_Request* request);
    void ExchangeRecv();
    void ExchangeWait(MPI_Request* request);

    //упаковывает вещественные поля части сетки
    int getPackSize(vec3<int> n1, vec3<int> n2);
    void PackData(vec3<int> n1, vec3<int> n2, double *& arr, Grid3d& grFrom);
    void UnPackData(vec3<int> n1, vec3<int> n2, double *& arr, Grid3d& grTo);

    vec3<int> getZero() { return 0; }
    vec3<int> getSumGuardAndMainDomainSizes() { return getGuardSize()+getMainDomainSize(); }
    typedef vec3<int>(MPIWorker::*F)();
    F arrF[4] = { &MPIWorker::getZero, &MPIWorker::getGuardSize,
        &MPIWorker::getSumGuardAndMainDomainSizes, &MPIWorker::getFullDomainSize };

    //возвращает смещение, необходимое для получения домена, которого касается область
    vec3<int> getDim(int i, int j, int k) {
        int x = i == 0 ? -1 : (i == 2 ? 1 : 0);
        int y = j == 0 ? -1 : (j == 2 ? 1 : 0);
        int z = k == 0 ? -1 : (k == 2 ? 1 : 0);
        return vec3<int>(x, y, z);
    }

    int getNum(int i, int j, int k) {
        return (i * 3 + j) * 3 + k;
    }

    vec3<int> getN1Send(int i, int j, int k) {
        vec3<int> dim = getDim(i, j, k);
        vec3<int> n1((this->*arrF[i])().x, (this->*arrF[j])().y, (this->*arrF[k])().z);
        if (dim.x == 1) n1.x += 1;
        if (dim.y == 1) n1.y += 1;
        if (dim.z == 1) n1.z += 1;
        return n1;
    }
    vec3<int> getN2Send(int i, int j, int k) {
        vec3<int> dim = getDim(i, j, k);
        vec3<int> n2((this->*arrF[i + 1])().x, (this->*arrF[j + 1])().y, (this->*arrF[k + 1])().z);
        if (dim.x == -1) n2.x += -1;
        if (dim.y == -1) n2.y += -1;
        if (dim.z == -1) n2.z += -1;
        return n2;
    }
    vec3<int> getN1Recv(int i, int j, int k) {
        vec3<int> dim = getDim(i, j, k);
        vec3<int> n1((this->*arrF[i])().x, (this->*arrF[j])().y, (this->*arrF[k])().z);
        n1 = n1 - dim*guardSize;
        if (dim.x == 1) n1.x += 1;
        if (dim.y == 1) n1.y += 1;
        if (dim.z == 1) n1.z += 1;
        return n1;
    }
    vec3<int> getN2Recv(int i, int j, int k) {
        vec3<int> dim = getDim(i, j, k);
        vec3<int> n2((this->*arrF[i + 1])().x, (this->*arrF[j + 1])().y, (this->*arrF[k + 1])().z);
        n2 = n2 - dim*guardSize;
        if (dim.x == -1) n2.x += -1;
        if (dim.y == -1) n2.y += -1;
        if (dim.z == -1) n2.z += -1;
        return n2;
    }
};