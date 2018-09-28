#move it to "../sln/x64/Release"

import subprocess;
import sys
import math
import matplotlib.pyplot as plt
import matplotlib as mpl

NUM_OF_PROCESSES = int(sys.argv[1])
FIELD_SOLVER = sys.argv[2];
N_PERIODS = int(sys.argv[3])

ITER_IN_PERIOD = -1
if (len(sys.argv)==5):
	ITER_IN_PERIOD = int(sys.argv[4])
	
	
NAME_FILE_IN = "..\\..\\..\\files\\running_wave\\parallel_results\\second_steps.csv"
DIR_OUT = "..\\..\\..\\files\\running_wave\\dispersion_results\\"
NAME_FILE_OUT = ""
NAME_GRAPH_OUT = ""
if (NUM_OF_PROCESSES==1):
	NAME_FILE_OUT=DIR_OUT+"consistent_"+FIELD_SOLVER+"_dispersion.csv"
	NAME_GRAPH_OUT=DIR_OUT+"consistent_"+FIELD_SOLVER+"_dispersion.png"
else:
	NAME_FILE_OUT=DIR_OUT+"parallel_"+FIELD_SOLVER+"_"+str(NUM_OF_PROCESSES)+"_domains"+"_dispersion.csv"
	NAME_GRAPH_OUT=DIR_OUT+"parallel_"+FIELD_SOLVER+"_"+str(NUM_OF_PROCESSES)+"_domains"+"_dispersion.png"
	
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

def callProcess(lambd_div_d):
	if (CALL == "cons"):
		process = subprocess.Popen(NAME_PROGRAM+" "+str(lambd_div_d)+" "+FIELD_SOLVER+" "+str(N_PERIODS)+" "+str(ITER_IN_PERIOD))
		process.wait()
	else:
		process = subprocess.Popen("mpiexec -n "+str(NUM_OF_PROCESSES)+" "+NAME_PROGRAM+" "+str(lambd_div_d)+" "+FIELD_SOLVER+" "+str(N_PERIODS)+" "+str(ITER_IN_PERIOD))
		process.wait()

def findV(arr, lambd):
	return LIGHT_SPEED+math.asin(arr[0])/(2*math.pi)
	
def writeFile(k, wTh, w):
	file=open(NAME_FILE_OUT, "a")
	file.write(str(k)+";"+str(wTh)+";"+str(w)+";"+"\n")
	file.close()

writeFile("k", "omega (th)", "omega (res)")
	
def dispPSTD(w, dt):
	return 2/LIGHT_SPEED/dt*math.sin(w*dt/2)

def dispPSTD_1(k, dt):
	return math.asin(LIGHT_SPEED*dt*k/2)*2/dt
	
arrK=[]; arrW=[]; arrWTh=[]; arrLambda=[]; arrDiffW=[]; arrErr=[]; arrErrK=[]
for i in range(NUM_OF_POINTS, 1, -1):
	print(i, "\n")
	
	lambd_div_d=pow(2, i)
	lambd = lambd_div_d*d
	T=lambd/LIGHT_SPEED
	
	print("lambda=", lambd, "\n")
		
	callProcess(lambd_div_d)
	dat=readFile()
	v=findV(dat, lambd)
	#v=0
	
	k=2*math.pi/lambd
	wTh=k*LIGHT_SPEED
	w=k*v
	
	arrK.insert(NUM_OF_POINTS-i, k)
	arrWTh.insert(NUM_OF_POINTS-i, wTh)
	arrW.insert(NUM_OF_POINTS-i, w)
	arrLambda.insert(NUM_OF_POINTS-i, lambd)
	arrDiffW.insert(NUM_OF_POINTS-i, w-wTh)
	
	writeFile(k, wTh, w)
	
fig = plt.figure()
ax = fig.add_subplot(111)

#ax.plot(arrK, arrWTh)
#ax.plot(arrK, arrW, "*")
#for i in range(len(arrK), 0, -1):
#	ax.annotate("lambda="+str(int(arrLambda[len(arrK)-i]/d)), xy=(arrK[len(arrK)-i]-0.05, arrW[len(arrK)-i]+0.02E12))

#ax.set_xlabel("k")
#ax.set_ylabel("w")

ax.plot(arrK, arrDiffW)
ax.plot(arrK, arrDiffW, "*")
#ax.plot(arrErrK, arrErr, 'o')
for i in range(len(arrK), 0, -1):
	ax.annotate("lambda="+str(int(arrLambda[len(arrK)-i]/d)), xy=(arrK[len(arrK)-i]-0.06, arrDiffW[len(arrK)-i]))

ax.set_xlabel("k")
ax.set_ylabel("разность полученного значения w и теоретического")

plt.tight_layout()

plt.savefig(NAME_GRAPH_OUT)

plt.show()


