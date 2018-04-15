#pragma once
#include "mpi.h"

class MPIWrapper {
public:
    static int MPISize() {
        int size;
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        return size;
    }
    static int MPIRank() {
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        return rank;
    }
    static void MPISend(double* buf, int size, int dest) {
        MPI_Send(buf, size, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
    }
    static void MPIRecv(double* buf, int size, int source) {
        MPI_Status status;
        MPI_Recv(buf, size, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &status);
    }
};