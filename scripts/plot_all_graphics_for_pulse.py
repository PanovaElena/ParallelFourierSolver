import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib.colors import LinearSegmentedColormap
import math
import sys

SIZE=2

#файл
name_files=[
	#"..\\files\\parallel_field_solver_console\\FirstStepsE.csv",
	"..\\files\\parallel_field_solver_console\\SecondStepsE.csv",
	"..\\files\\field_solver_test_pulse_E\\iter_40.csv",
	]

dir="..\\files\\graphics_script\\"
name_graphics=[
	#"Поле после выполнения 30 итераций",
	"Поле после выполнения 40 итераций параллельно",
	"Поле после выполнения 40 итераций последовательно",
]

i=3
j=0
while (j<SIZE):
	name_files.insert(i,"..\\files\\parallel_field_solver_console\\SecondStepsStartE"+str(j)+".csv")
	name_files.insert(i+1,"..\\files\\parallel_field_solver_console\\SecondStepsFinalE"+str(j)+".csv")
	name_files.insert(i+2,"..\\files\\parallel_field_solver_console\\rank_"+str(j)+"_iter_9_E.csv")
	name_graphics.insert(i, "Поле перед выполнением параллельной части на "+str(j)+" домене")
	name_graphics.insert(i+1, "Поле после выполнения параллельной части на "+str(j)+" домене")
	name_graphics.insert(i+2, "Поле перед занулением граничных областей на "+str(j)+" домене")
	
	i=i+3
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

	plt.savefig(dir+name+".png")
	
i=0;
while (i<len(name_files)):
	file=open(name_files[i], 'r')
	dat = UnPack(file)
	file.close()

	plot(name_graphics[i], dat)
	
	i=i+1

	
file=open(name_files[0], 'r')
datPar = UnPack(file)
file.close()

file=open(name_files[1], 'r')
datCons = UnPack(file)
file.close()

datError=[]
datErrorAbs=[]
datErrorOtn=[]

i=0
for line in datPar:
	j=0
	tmpStr=[]
	tmpStrAbs=[]
	tmpStrOtn=[]
	for elem in datPar[i]:
		tmpStr.insert(j, float(datCons[i][j])-float(datPar[i][j]))
		tmpStrAbs.insert(j, abs(float(datCons[i][j])-float(datPar[i][j])))
		a=0
		if (abs(float(datCons[i][j]))>1e-6 and abs(float(datCons[i][j])-float(datPar[i][j]))>1e-6):
			a=abs(float(datCons[i][j])-float(datPar[i][j]))/float(datCons[i][j])*100;
		tmpStrOtn.insert(j, a)
		j=j+1
	datError.insert(i, tmpStr)
	datErrorAbs.insert(i, tmpStrAbs)
	datErrorOtn.insert(i, tmpStrOtn)
	i=i+1

plot("Абсолютная погрешность", datErrorAbs)
plot("Относительная погрешность", datErrorOtn)
plot("Разность последовательного и параллельного решения", datError)
	

			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
