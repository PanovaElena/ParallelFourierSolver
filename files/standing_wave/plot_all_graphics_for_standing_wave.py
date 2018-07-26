import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib.colors import LinearSegmentedColormap
import math
import sys

NUM_OF_PROCESSES=int(sys.argv[1])

arrLetters=['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z']
graphsForOneProcess=4
graphsFirst=1
graphsCommon=graphsForOneProcess+graphsFirst+graphsFirst

parRes=NUM_OF_PROCESSES*graphsForOneProcess+graphsFirst
consRes=parRes+1

dir="parallel_results\\"
#файл
name_files=[
	dir+"first_steps.csv",
	dir+"second_steps.csv",
	"consistent_results\\E\\iter_800_coord_1.csv",
	]

dirGraphs="standing_wave_graphics\\"
name_graphics=[
	arrLetters[0]+"_Поле после выполнения 600 итераций",
	arrLetters[parRes]+"_Поле после выполнения 800 итераций параллельно",
	arrLetters[consRes]+"_Поле после выполнения 800 итераций последовательно",
	arrLetters[consRes+1]+"_Разность последовательного и параллельного решения",
	arrLetters[consRes+2]+"_Абсолютная погрешность",
	arrLetters[consRes+3]+"_Относительная погрешность",
]

i=graphsFirst
j=0
while (j<NUM_OF_PROCESSES):
	name_files.insert(i,dir+"parallel_steps_start_"+str(j)+".csv")
	name_files.insert(i+1,dir+"rank_"+str(j)+"_iter_199.csv")
	name_files.insert(i+2,dir+"after_exchange_rank_"+str(j)+".csv")
	name_files.insert(i+3,dir+"parallel_steps_final_"+str(j)+".csv")
	name_graphics.insert(i, arrLetters[j*graphsForOneProcess+graphsFirst]+"_Поле перед выполнением параллельной части на "+str(j)+" домене")
	name_graphics.insert(i+1, arrLetters[j*graphsForOneProcess+graphsFirst+1]+"_Поле перед обменом граничных областей на "+str(j)+" домене")
	name_graphics.insert(i+2, arrLetters[j*graphsForOneProcess+graphsFirst+2]+"_Поле после обмена граничных областей на "+str(j)+" домене")
	name_graphics.insert(i+3, arrLetters[j*graphsForOneProcess+graphsFirst+3]+"_Поле после выполнения параллельной части на "+str(j)+" домене")
	
	i=i+graphsForOneProcess
	j=j+1

def UnPack(file):
	res=[]
	i=0
	for line in file:
		res.insert(i,line.split(';'))
		res[i].pop()
		j=0
		for elem in res[i]:
			res[i][j]=float(res[i][j])
			j=j+1
		i=i+1
	return res
	
def plot(name, dat):
	fig = plt.figure()
	ax = fig.add_subplot(111)

	cm='gray'
	#cs = ax.contourf(dat, cmap=cm)
	cs = ax.imshow(dat, cmap=cm)
	fig.colorbar(cs, ax=ax)
	ax.set_title(name)

	plt.tight_layout()

	plt.savefig(dirGraphs+name+".png")	
	
i=0;
while (i<len(name_files)):
	print(name_graphics[i])
	file=open(name_files[i], 'r')
	dat = UnPack(file)
	file.close()

	plot(name_graphics[i], dat)
	
	i=i+1

file=open(name_files[parRes], 'r')
datPar = UnPack(file)
file.close()

file=open(name_files[consRes], 'r')
datCons = UnPack(file)
file.close()

datError=[]
datErrorAbs=[]
datErrorOtn=[]

i=0
eps=1e-5
for line in datPar:
	j=0
	tmpStr=[]
	tmpStrAbs=[]
	tmpStrOtn=[]
	for elem in datPar[i]:
		tmpStr.insert(j, float(datCons[i][j])-float(datPar[i][j]))
		tmpStrAbs.insert(j, abs(float(datCons[i][j])-float(datPar[i][j])))
		a=0
		if (abs(float(datCons[i][j]))>eps and abs(float(datCons[i][j])-float(datPar[i][j]))>eps):
			a=abs(float(datCons[i][j])-float(datPar[i][j]))/float(datCons[i][j])*100;
		tmpStrOtn.insert(j, a)
		j=j+1
	datError.insert(i, tmpStr)
	datErrorAbs.insert(i, tmpStrAbs)
	datErrorOtn.insert(i, tmpStrOtn)
	i=i+1

plot(name_graphics[NUM_OF_PROCESSES*graphsForOneProcess+graphsCommon-3], datError)
plot(name_graphics[NUM_OF_PROCESSES*graphsForOneProcess+graphsCommon-2], datErrorAbs)
plot(name_graphics[NUM_OF_PROCESSES*graphsForOneProcess+graphsCommon-1], datErrorOtn)

x=[]
y=[]
i=0
while (i<len(datError)):
	x.insert(i,i)
	y.insert(i, datError[int(len(datError)/2)][i])
	i=i+1
	
fig = plt.figure()
ax = fig.add_subplot(111)

ax.plot(x,y)
ax.set_title("погрешность по X")

plt.tight_layout()

plt.savefig(dirGraphs+"погрешность по X"+".png")	
	