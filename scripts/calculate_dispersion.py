import subprocess;
import sys
import math

NUM_OF_PROCESSES = int(sys.argv[1])
NAME_FILE_IN = "..\\files\\running_wave\\parallel_results\\second_steps.csv"
NAME_FILE_OUT = "..\\files\\running_wave\\dispersion.csv"
NAME_GRAPH_OUT = "..\\files\\running_wave\\dispersion.png"
NAME_PROGRAM = "..\\sln\\x64\\Release\\ParallelFieldSolver_RunningWave.exe"

file=open(NAME_FILE_OUT, "w")
file.close()

d=1
N = 128
LIGHT_SPEED = 2997924580000
dt = d / (LIGHT_SPEED*math.sqrt(2) * 8)

MAX_LAMBDA = N/NUM_OF_PROCESSES
MIN_K=2*math.pi/MAX_LAMBDA
MAX_K=math.pi/d
MIN_LAMBDA=2*math.pi/MAX_K

print("MAX_LAMBDA=", MAX_LAMBDA, "\n")
print("MIN_LAMBDA=", MIN_LAMBDA, "\n")
print("MAX_K=", MAX_K, "\n")
print("MIN_K=", MIN_K, "\n")

NUM_OF_POINTS = 5
STEP = (MAX_K-MIN_K)/NUM_OF_POINTS

def findNextK(i):
	return MIN_K+STEP*i

def readFile():
	dat=[]
	with open(NAME_FILE_IN) as file:
		for x in file: 
			dat.append(float(x))
	return dat

def callProcess(lambd, t):
	process = subprocess.Popen("mpiexec -n " + str(NUM_OF_PROCESSES) + " " + NAME_PROGRAM + " " + str(lambd/d) + " 0 " + str(t/dt))
	process.wait()

def findV(arr, minPlaceInd, t):
	min=arr[minPlaceInd]
	for i in range(minPlaceInd-1, 0, -1):
		if (arr[i]>=0):
			continue;
		if (min>arr[i]):
			min=arr[i]
		else:
			print("min=", min, "\n")
			return (minPlaceInd-i)/t
			
def calcMinPlaceInd(lambd):
	l=3/4*lambd
	return int(l/d)
	
def writeFile(v):
	file=open(NAME_FILE_OUT, "a")
	file.write(str(v)+"\n")
	file.close()

arrK=[]
arrW=[]
arrWTh=[]
for i in range(0, NUM_OF_POINTS):
	print(i, "\n")

	k=findNextK(i)
	lambd = 2*math.pi/k
	t=lambd/2/LIGHT_SPEED
	minPlaceInd = calcMinPlaceInd(lambd)
	
	print("k=", k, "\n")
	print("lambda=", lambd, "\n")
	print("t=", t, "\n")
	print("t/dt=", t/dt, "\n")
	print("minPlaceInd=", minPlaceInd, "\n")
	
	
	callProcess(lambd, t)
	dat=readFile()
	v=findV(dat, minPlaceInd, t)
	
	arrK.insert(i, k)
	arrWTh.insert(i, k*LIGHT_SPEED)
	arrW.insert(i, k*v)
	
	writeFile(v)
	


