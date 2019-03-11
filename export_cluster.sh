#!/bin/sh

module load intel2017
export INTEL="/common/intel_2017/compilers_and_libraries_2017.0.098/linux"
env | grep INTEL
export I_MPI_ROOT="/common/intel_2017/compilers_and_libraries_2017.0.098/linux/mpi"
env | grep I_MPI_ROOT
export MKL_ROOT="/common/intel_2017/compilers_and_libraries_2017.0.098/linux/mkl"
env | grep MKL_ROOT