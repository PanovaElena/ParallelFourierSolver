#!/bin/sh

#SBATCH -N 1
#SBATCH -w node102
#SBATCH --time 1000

srun python ./test_sequential.py

wait