import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib.colors import LinearSegmentedColormap
import math
import sys

#файл
name_files=[
	"..\\files\\parallel_field_solver_console\\FirstStepsE.csv",
	"..\\files\\parallel_field_solver_console\\SecondStepsStartELeft.csv",
	"..\\files\\parallel_field_solver_console\\SecondStepsStartERight.csv",
	"..\\files\\parallel_field_solver_console\\rank_0_iter_9_E.csv",
	"..\\files\\parallel_field_solver_console\\rank_1_iter_9_E.csv",
	"..\\files\\parallel_field_solver_console\\SecondStepsFinalELeft.csv",
	"..\\files\\parallel_field_solver_console\\SecondStepsFinalERight.csv",
	"..\\files\\parallel_field_solver_console\\SecondStepsE.csv",
	"..\\files\\field_solver_test_pulse_E\\iter_40.csv"
	]

dir="..\\files\\graphics_script\\"
name_graphics=[
	"Поле после выполнения 30 итераций",
	"Поле на левом домене перед выполнением параллельной части",
	"Поле на правом перед выполнением параллельной части",
	"Поле на левом домене перед занулением граничных областей",
	"Поле на правом домене перед занулением граничных областей",
	"Поле на левом домене после выполнения параллельной части",
	"Поле на правом домене после выполнения параллельной части",
	"Поле после выполнения 40 итераций параллельно",
	"Поле после выполнения 40 итераций последовательно"
]

def UnPack(file):
	res=[]
	i=0
	for line in file:
		res.insert(i,line.split(';'))
		res[i].pop()
		for elem in res[i]:
			elem=float(elem)
		i=i+1
	return res
	
def plot(name, dat):
	fig = plt.figure()
	ax = fig.add_subplot(111)

	cm='gray'
	cs = ax.contourf(dat, cmap=cm)
	#cs = ax.imshow(dat)
	fig.colorbar(cs, ax=ax)
	ax.set_title(name)

	plt.tight_layout()

	plt.savefig(dir+name+".png")
	
i=0;
while (i<9):
	file=open(name_files[i], 'r')
	dat = UnPack(file)
	file.close()

	plot(name_graphics[i], dat)
	
	i=i+1

	
file=open(name_files[7], 'r')
datPar = UnPack(file)
file.close()

file=open(name_files[8], 'r')
datCons = UnPack(file)
file.close()

datError=[]

i=0
for line in datPar:
	j=0
	tmpStr=[]
	for elem in datPar[i]:
		tmpStr.insert(j, abs(float(datPar[i][j])-float(datCons[i][j])))
		j=j+1
	datError.insert(i, tmpStr)
	i=i+1

plot("Погрешность", datError)
	

			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
