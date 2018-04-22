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
    static void MPISend(double*& buf, int size, int dest, int tag) {
        MPI_Request request;
        MPI_Isend(buf, size, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD, &request);
    }
    static void MPIRecv(double*& buf, int size, int source, int tag) {
        MPI_Status status;
        MPI_Recv(buf, size, MPI_DOUBLE, source, tag, MPI_COMM_WORLD, &status);
    }
    static void MPIInitialize() {
        int argc;
        char** argv;
        MPI_Init(&argc, &argv);
    }
    static void MPIFinalize() {
        MPI_Finalize();
    }
};