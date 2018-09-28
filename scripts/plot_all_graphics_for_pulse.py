import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib.colors import LinearSegmentedColormap
import math
import sys

SIZE=4

#файл
name_files=[
	"..\\files\\parallel_field_solver_console\\FirstStepsE.csv",
	"..\\files\\parallel_field_solver_console\\SecondStepsE.csv",
	"..\\files\\field_solver_test_pulse_E\\iter_40.csv",
	]

dir="..\\files\\graphics_script\\"
name_graphics=[
	"Поле после выполнения 30 итераций",
	"Поле после выполнения 40 итераций параллельно",
	"Поле после выполнения 40 итераций последовательно",
]

i=3;
while (i<SIZE):
	name_files.insert(i,"..\\files\\parallel_field_solver_console\\SecondStepsStartE"+str(i)+".csv")
	name_files.insert(i+1,"..\\files\\parallel_field_solver_console\\SecondStepsFinalE"+str(i)+".csv")
	name_files.insert(i+2,"..\\files\\parallel_field_solver_console\\rank_"+str(i)+"_iter_9_E.csv")
	name_graphics.insert(i, "Поле перед выполнением параллельной части на "+str(i)+"домене")
	name_graphics.insert(i+1, "Поле после выполнения параллельной части на "+str(i)+"домене")
	name_graphics.insert(i+2, "Поле перед занулением граничных областей на "+str(i)+"домене")
	
	i=i+3

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
	
# i=0;
# while (i<len(name_files)):
	# file=open(name_files[i], 'r')
	# dat = UnPack(file)
	# file.close()

	# plot(name_graphics[i], dat)
	
	# i=i+1

	
file=open(name_files[1], 'r')
datPar = UnPack(file)
file.close()

file=open(name_files[2], 'r')
datCons = UnPack(file)
file.close()

datError=[]
datErrorAbs=[]

i=0
for line in datPar:
	j=0
	tmpStr=[]
	tmpStr1=[]
	for elem in datPar[i]:
		tmpStr.insert(j, float(datCons[i][j])-float(datPar[i][j]))
		tmpStr1.insert(j, abs(float(datCons[i][j])-float(datPar[i][j])))
		j=j+1
	datError.insert(i, tmpStr)
	datErrorAbs.insert(i, tmpStr1)
	i=i+1

plot("Погрешность", datError)
plot("Погрешность по модулю", datErrorAbs)
	

			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
