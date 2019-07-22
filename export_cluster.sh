#!/bin/sh

module load intel
export I_MPI_ROOT="/common/intel_2017/compilers_and_libraries/linux/mpi"
env | grep I_MPI_ROOT
