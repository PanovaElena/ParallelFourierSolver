
import math
import subprocess
import os
import sys
sys.path.append("../../scripts/")
import plot_graph as pg

OS=sys.argv[1]
FOLDER=""
EXE=""
NAME_MPI="/common/intel/impi/4.1.3.048/intel64/bin/mpirun"
if (OS=="Win"):
	NAME_MPI="mpiexec"
	FOLDER="/Release/"
	EXE=".exe"

DIR_SCRIPT = "./"+os.path.dirname(sys.argv[0]) 
DIR_RES="./../../files/spherical_wave/"

NAME_CONS_PROGRAM = "\""+DIR_SCRIPT+"/../../build/src/Examples/SphericalWave/ConsistentFieldSolver_SphericalWave/"+FOLDER+"/sphericalWave_consistent"+"\""

NAME_FILE_CONS  = DIR_SCRIPT+"/consistent_results/E/cons_res.csv"

LIST_FIELD_SOLVERS = ["PSTD", "PSATD"]

LIGHT_SPEED = 2997924580000
D=LIGHT_SPEED
COURANT_CONDITION_PSTD = math.sqrt(2)*D/(LIGHT_SPEED*math.pi)

DT=[0.1] #, COURANT_CONDITION_PSTD/4, COURANT_CONDITION_PSTD/2]
SOLVER=["PSTD", "PSATD"]

def calc_error(data_1, data_2):	
	error=0
	for i in range(len(data_1)):
		for j in range(len(data_1[i])):
			if (abs(data_1[i][j]-data_2[i][j])>abs(error)): error=data_1[i][j]-data_2[i][j]
	return error
	
def calc_ampl(data):
	ampl=0
	for i in range(len(data)):
		for j in range(len(data[i])):
			if (abs(data[i][j])>ampl):
				ampl=abs(data[i][j])
	return ampl
	
def calc_error_arr(data_1, data_2):	
	error=[]
	for i in range(len(data_1)):
		tmp=[]
		for j in range(len(data_1[i])):
			tmp.append(data_1[i][j]-data_2[i][j])
		error.append(tmp)			
	return error


data_graph=[]
for dt in DT:

	#nIter=int(50*COURANT_CONDITION_PSTD/dt)
	nIter=500
	command_args = " --nx "+str(128)+" --ny "+str(128)+" --nz "+str(1)+\
					" --nCons "+str(nIter)+\
					" --solver %s"+" --mask "+"smooth"+ " --dt "+str(dt)
					
					
	process = subprocess.Popen(NAME_CONS_PROGRAM+" "+command_args % "PSTD", shell=True)
	process.wait()
	
	data_PSTD=pg.readFile_2d(NAME_FILE_CONS)
	print("PSTD: ",calc_ampl(data_PSTD)) 
	
	process_2 = subprocess.Popen(NAME_CONS_PROGRAM+" "+command_args % "PSATD", shell=True)
	process_2.wait()
	
	data_PSATD=pg.readFile_2d(NAME_FILE_CONS)
	print("PSATD: ",calc_ampl(data_PSATD)) 
	
	process_3 = subprocess.Popen(NAME_CONS_PROGRAM+" "+command_args % "FDTD", shell=True)
	process_3.wait()
	
	data_FDTD=pg.readFile_2d(NAME_FILE_CONS)
	print("FDTD: ",calc_ampl(data_FDTD)) 
	
	#data_graph.append(calc_error(data_PSATD, data_PSTD))
	
	pg.plot_2d("./", "PSTD_dt_%s_nIter_%s" % (dt, nIter), data_PSTD)
	pg.plot_2d("./", "PSATD_dt_%s_nIter_%s" % (dt, nIter), data_PSATD)
	pg.plot_2d("./", "FDTD_dt_%s_nIter_%s" % (dt, nIter), data_FDTD)
	pg.plot_2d("./", "PSATD_PSTD_error_graph_dt_%s" % dt, calc_error_arr(data_PSATD, data_PSTD))
	pg.plot_2d("./", "PSATD_FDTD_error_graph_dt_%s" % dt, calc_error_arr(data_FDTD, data_PSTD))
	
#pg.plot_1d("./", "PSATD_PSTD_cons_error", data_graph, DT, "dt", "error", True)	
					
					
					
					