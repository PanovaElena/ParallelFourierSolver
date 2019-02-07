
import math
import subprocess
import os
import sys
sys.path.append("../../scripts/")
import plot_graph as pg



SOLVER=sys.argv[1]
OS=sys.argv[2]
FOLDER=""
EXE=""
NAME_MPI="/common/intel/impi/4.1.3.048/intel64/bin/mpirun"
if (OS=="Win"):
	NAME_MPI="mpiexec"
	FOLDER="/Release/"
	EXE=".exe"
	
DIR_SCRIPT = "./"+os.path.dirname(sys.argv[0]) 

NAME_PROGRAM_PAR = "\""+DIR_SCRIPT+"/../../build/src/Examples/RunningWave/ParallelFieldSolver_RunningWave/"+FOLDER+"/runningWave_parallel"+"\""
NAME_PROGRAM_CONS = "\""+DIR_SCRIPT+"/../../build/src/Examples/RunningWave/ConsistentFieldSolver_RunningWave/"+FOLDER+"/runningWave_consistent"+"\""

NAME_FILE_CONS = DIR_SCRIPT+"/consistent_results/E/cons_res.csv"
NAME_FILE_PAR = DIR_SCRIPT+"/parallel_results/second_steps.csv"

NUM_PROCESSES=2
N_ITER=3
DT=6e-13

def calc_error(data_cons, data_par):	
	error=0
	for i in range(len(data_par)):
		if (abs(data_par[i]-data_cons[i])>abs(error)): error=abs(data_par[i]-data_cons[i])
	return error
	
data_graph=[]
D=math.sqrt(6)*math.pi
#N_GUARDS=[10, 20, 30, 50, 70, 90, 120, 150, 180, 220, 256]
N_GUARDS=[20, 40, 60, 100, 140, 180, 240, 300, 360, 440, 512]

with open("error_"+SOLVER+"_n_guards.csv", 'w') as file:
	for n_guards in N_GUARDS:
		
		command_args = " --nx "+str(256)+" --ny "+str(1)+" --nz "+str(256)+\
						" --nCons %s --nPar %s --guard "+str(n_guards)+\
						" --solver "+SOLVER+\
						" --dt "+str(DT)
						
		process = subprocess.Popen(NAME_MPI + " -n "+str(NUM_PROCESSES)+" "+NAME_PROGRAM_PAR+" "+command_args%(str(0), str(N_ITER)), shell=True)
		process.wait()
		
		data_par=pg.readFile_1d(NAME_FILE_PAR)
		
		process = subprocess.Popen(NAME_PROGRAM_CONS+" "+command_args%(str(N_ITER), str(0)), shell=True)
		process.wait()
		
		data_cons=pg.readFile_1d(NAME_FILE_CONS)
		
		error=calc_error(data_cons, data_par)
		data_graph.append(error)
		
		file.write(str(n_guards)+ ";"+ str(error)+"\n")
	
	
pg.plot_1d("./", "error_"+SOLVER+"_n_guards", data_graph, N_GUARDS, "n_guards", "error", True)	