#move it to "../sln/x64/Release"

import subprocess;
import sys
import math
import matplotlib.pyplot as plt
import matplotlib as mpl

NUM_OF_PROCESSES = int(sys.argv[1])
FIELD_SOLVER = sys.argv[2];
LAMBDA = int(sys.argv[3])
ITER_IN_PERIOD = int(sys.argv[4])
	
	
NAME_FILE_IN = "..\\..\\..\\files\\running_wave\\parallel_results\\second_steps.csv"
DIR_OUT = "..\\..\\..\\files\\running_wave\\energy_results\\"
NAME_FILE_OUT = ""
NAME_GRAPH_OUT = ""
if (NUM_OF_PROCESSES==1):
	NAME_FILE_OUT=DIR_OUT+"consistent_"+FIELD_SOLVER+"_energy.csv"
	NAME_GRAPH_OUT=DIR_OUT+"consistent_"+FIELD_SOLVER+"_energy.png"
else:
	NAME_FILE_OUT=DIR_OUT+"parallel_"+FIELD_SOLVER+"_"+str(NUM_OF_PROCESSES)+"_domains"+"_energy.csv"
	NAME_GRAPH_OUT=DIR_OUT+"parallel_"+FIELD_SOLVER+"_"+str(NUM_OF_PROCESSES)+"_domains"+"_energy.png"
	
NAME_PROGRAM = "ParallelFieldSolver_RunningWave.exe"

CALL = "par"
if (NUM_OF_PROCESSES==1):
	NAME_FILE_IN = "..\\..\\..\\files\\running_wave\\consistent_results\\E\\cons_res.csv"
	NAME_PROGRAM = "ConsistentFieldSolver_RunningWave.exe"
	CALL = "cons"

file=open(NAME_FILE_OUT, "w")
file.close()

d = math.sqrt(6)*math.pi
N = 256
GUARD=16
LIGHT_SPEED = 2997924580000

MAX_LAMBDA = N/NUM_OF_PROCESSES
NUM_OF_POINTS = int(math.log(MAX_LAMBDA)/math.log(2))

COURANT_CONDITION_PSTD = math.sqrt(2)*d/(LIGHT_SPEED*math.pi)

def readFile():
	dat=[]
	with open(NAME_FILE_IN) as file:
		for x in file: 
			dat.append(float(x))
	return dat

def callProcess(nPeriods):
	if (CALL == "cons"):
		process = subprocess.Popen(NAME_PROGRAM+" "+str(LAMBDA)+" "+FIELD_SOLVER+" "+str(nPeriods)+" "+str(ITER_IN_PERIOD))
		process.wait()
	else:
		process = subprocess.Popen("mpiexec -n "+str(NUM_OF_PROCESSES)+" "+NAME_PROGRAM+" "+str(LAMBDA)+" "+FIELD_SOLVER+" "+str(nPeriods)+" "+str(ITER_IN_PERIOD))
		process.wait()

def findEnergy(arr):
	s=0
	for i in range(0, N):
		s=s+arr[i]*arr[i]
	return s*d
	
def findEnergyTh(lambd, t):
	k=2*math.pi/lambd
	omega=LIGHT_SPEED*k
	s=0
	for i in range(0, N):
		xi=i*d
		s=s+math.sin(k*xi-omega*t)*math.sin(k*xi-omega*t)
	return s*d
	
def writeFile(nPeriods, energyTh, energy, t):
	file=open(NAME_FILE_OUT, "a")
	file.write(str(nPeriods)+";"+str(energyTh)+";"+str(energy)+";"+str(t)+";"+"\n")
	file.close()

writeFile("nPeriods", "energy (th)", "energy", "t")
	
arrEnergy=[]; arrEnergyTh=[]; arrNPeriods=[]; arrDiffEnergy=[]
for nPeriods in range(0, 31, 5):
	print(nPeriods, "\n")
	
	lambd_div_d=LAMBDA
	lambd = lambd_div_d*d
	T=lambd/LIGHT_SPEED
	t=T*nPeriods
		
	callProcess(nPeriods)
	dat=readFile()
	energy=findEnergy(dat)
	energyTh=findEnergyTh(lambd, t)
	#energy=0
	
	arrEnergy.insert(nPeriods, energy)
	arrEnergyTh.insert(nPeriods, energyTh)
	arrDiffEnergy.insert(nPeriods, energy-energyTh)
	arrNPeriods.insert(nPeriods, nPeriods)
	
	writeFile(nPeriods, energyTh, energy, t)

fig = plt.figure()
ax = fig.add_subplot(111)

ax.plot(arrNPeriods, arrDiffEnergy)
ax.plot(arrNPeriods, arrDiffEnergy, "*")
	
plt.tight_layout()

plt.savefig(NAME_GRAPH_OUT)

plt.show()