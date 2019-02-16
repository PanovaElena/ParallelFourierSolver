import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib.colors import LinearSegmentedColormap
import pylab as pl


def readFile1d(file):
	data=[]
	with open(file) as file:
		for x in file: 
			try:
				if(x!="\n" and x!=" "):
					data.append(float(x))
				else:
					break
			except:
				print("ERROR, can't convert data to float")
	return data
	
def readFile2d(file):
	data=[]
	with open(file) as file:
		for line in file: 
			try:
				arr=line.strip().split(';'); arr.pop()
				row=[float(x) for x in arr]
				data.append(row)
			except:
				print("ERROR, can't convert data to float")
	return data

	
def plot1d(dir, name, data, arg=[], xlabel="", ylabel="", points=False, _log=False):
	if (arg==[]): arg=range(0, len(data))
	fig = plt.figure()
	ax = fig.add_subplot(111)
	if (_log):
		ax.semilogy(arg, data)
		if (points==True): ax.semilogy(arg, data, "*")
	else:
		ax.plot(arg, data)
		if (points==True): ax.plot(arg, data, "*")
	pl.xlabel(xlabel)
	pl.ylabel(ylabel)
	#ax.set_title(name)
	plt.tight_layout()
	plt.savefig(dir+"/"+name+".png")
	
def plot2d(dir, name, data):
	fig = plt.figure()
	ax = fig.add_subplot(111)
	cs = ax.imshow(data, cmap='gray')
	fig.colorbar(cs, ax=ax)
	#ax.set_title(name)
	plt.tight_layout()
	plt.savefig(dir+"/"+name+".png")
	

def plotError1d(dir, name, data1, data2):
	data=[]
	for i in range(len(data1)):
		data.append(data1[i]-data2[i])
	plot1d(dir, name, data)
	
def plotError2d(dir, name, data1, data2):
	data=[]
	for i in range(len(data1)):
		tmp=[]
		for j in range(len(data1[i])):
			tmp.append(data1[i][j]-data2[i][j])
		data.append(tmp)
	plot2d(dir, name, data)