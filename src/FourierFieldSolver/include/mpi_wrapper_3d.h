#pragma once
#include "mpi_wrapper.h"
#include "vec.h"
#include "status.h"

class MPIWrapper3d {
    int sizeX = 1, sizeY = 1, sizeZ = 1;
public:
    MPIWrapper3d() {}
    MPIWrapper3d(vec3<int> np) {
        sizeX = np.x;
        sizeY = np.y;
        sizeZ = np.z;
    }

    Status CheckAndSetSize(int _sizeX, int _sizeY, int _sizeZ) {
        if (_sizeX*_sizeY*_sizeZ != MPIWrapper::MPISize())
            return Status::ERROR;
        SetSize(_sizeX, _sizeY, _sizeZ);
        return Status::OK;
    }

    void SetSize(int _sizeX, int _sizeY, int _sizeZ) {
        sizeX = _sizeX;
        sizeY = _sizeY;
        sizeZ = _sizeZ;
    }

    vec3<int> MPISize() {
        return vec3<int>(sizeX, sizeY, sizeZ);
    }
    int getScalarRank(vec3<int> rank) {
        return (rank.x*sizeY + rank.y)*sizeZ + rank.z;
    }
    int getTag(vec3<int> dim) {
        int N = 3;
        dim = dim + vec3<int>(N);
        return (dim.x*N + dim.y)*N + dim.z;
    }
    vec3<int> getVecRank(int rank) {
        int z = rank%sizeZ;
        int tmp = rank / sizeZ;
        int y = tmp%sizeY;
        int x = tmp / sizeY;
        return vec3<int>(x, y, z);
    }
    vec3<int> MPIRank() {
        int rank = MPIWrapper::MPIRank();
        return getVecRank(rank);
    }
    void MPIISend(double*& buf, int size, vec3<int> dest, int tag, MPI_Request& request) {
        MPIWrapper::MPIISend(buf, size, getScalarRank(dest), tag, request);
    }
    void MPIRecv(double*& buf, int size, vec3<int> source, int tag) {
        MPIWrapper::MPIRecv(buf, size, getScalarRank(source), tag);
    }
};
