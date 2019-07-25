#!/bin/bash

#SBATCH -A SNIC2018-1-38
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH --constraint=skylake
#SBATCH --time=24:00:00

## -n количество запускаемых процессов (обычно 2N)
## количесво памяти и ядер на ноде на сайте: https://www.hpc2n.umu.se/resources/hardware/kebnekaise
## --constraint: skylake (for SkyLake), broadwell (for Broadwell) в части batch (по умолчанию)

echo "start"
export KMP_AFFINITY="granularity=fine,compact,1,0"
schemes=(ompstatic ompdynamic manualdist sorteddynamic dynamicdist particlesdist)
#schemes=(particlesdist)
name='test2d-quarl'
echo "end"
for((count=0; count<1; count++))
do
    for thread in 1 2 4 7 8 14 28 56
    do
        export OMP_NUM_THREADS=$thread
        echo $thread
        for scheme in ${schemes[*]}
        do
            for index in '1' '2' '3'
            do
                echo $scheme $index$name'.txt'
                mpirun -n 1 ./Picador -w $scheme $index$name'.txt'
                mpirun -n 1 python move_result.py $scheme $index
            done
        done
    done
done
mpirun -n 1 python run_parse.py $name
wait

## sbatch XXXXXX.sbatch для запуска
## ssqueue посмотреть очередь
## scancel NNNNNN удалить задачу