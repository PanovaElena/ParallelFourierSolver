#!/bin/bash

#SBATCH -A SNIC2018-1-38
#SBATCH -N 1
#SBATCH --time=5:00:00

export KMP_AFFINITY="granularity=fine,compact,1,0"

srun python ./scal_omp.py
wait