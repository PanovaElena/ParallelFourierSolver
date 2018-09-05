import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib.colors import LinearSegmentedColormap
import math
import sys

NUM_OF_PROCESSES=int(sys.argv[1])
DIR_FROM=str(sys.argv[2])
DIR_TO=str(sys.argv[3])

graphsForOneProcess=4
graphsFirst=1
graphsCommon=graphsForOneProcess+graphsFirst+graphsFirst

parRes=NUM_OF_PROCESSES*graphsForOneProcess+graphsFirst
consRes=parRes+1

dir=DIR_FROM
#файл
name_files=[
	dir+"first_steps.csv",
	dir+"second_steps.csv",
	"consistent_results\\E\\cons_res.csv",
	]

dirGraphs=DIR_TO
name_graphics=[
	"Поле после выполнения последовательной части",
	"Поле после выполнения параллельной части",
	"Поле при последовательном счете",
	"Разность последовательного и параллельного решения",
	"Абсолютная погрешность",
]

i=graphsFirst
for j in range(0, NUM_OF_PROCESSES):
	name_files.insert(i,dir+"parallel_steps_start_"+str(j)+".csv")
	name_files.insert(i+1,dir+"iter_rank_"+str(j)+".csv")
	name_files.insert(i+2,dir+"after_exchange_rank_"+str(j)+".csv")
	name_files.insert(i+3,dir+"parallel_steps_final_"+str(j)+".csv")
	name_graphics.insert(i, "Поле перед выполнением параллельной части на "+str(j)+" домене")
	name_graphics.insert(i+1, "Поле перед обменом граничных областей на "+str(j)+" домене")
	name_graphics.insert(i+2, "Поле после обмена граничных областей на "+str(j)+" домене")
	name_graphics.insert(i+3, "Поле после выполнения параллельной части на "+str(j)+" домене")
	
	i=i+graphsForOneProcess
	
def readFile(name):
	dat=[]
	with open(name) as file:
		for x in file: 
			dat.append(float(x))
	return dat
	
def plot(name, dat):
	fig = plt.figure()
	ax = fig.add_subplot(111)

	ax.plot(range(0, len(dat)), dat)
	ax.set_title(name)

	plt.tight_layout()

	plt.savefig(dirGraphs+name+".png")
	
def calcErr(datPar, datCons):	
	err=[];	
	for i in range(0, len(datPar)):
		err.insert(i, float(datCons[i])-float(datPar[i]))
	return err


def calcAbsErr(datPar, datCons):	
	absErr=[];
	for i in range(0, len(datPar)):
		absErr.insert(i, abs(float(datCons[i])-float(datPar[i])))
	return absErr
	


for i in range(0,len(name_files)):
	print(name_graphics[i])
	
	dat = readFile(name_files[i])

	plot(name_graphics[i], dat)

	
datPar = readFile(name_files[parRes])
datCons = readFile(name_files[consRes])

err=calcErr(datPar, datCons)
absErr=calcAbsErr(datPar, datCons)

plot(name_graphics[NUM_OF_PROCESSES*graphsForOneProcess+graphsCommon-3], err)
plot(name_graphics[NUM_OF_PROCESSES*graphsForOneProcess+graphsCommon-2], absErr)
	