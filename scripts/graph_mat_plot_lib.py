import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib.colors import LinearSegmentedColormap
import math
import sys

#файл
file=open(sys.argv[1], 'r')
filePath = '.\\..\\files\\graphics\\'
graphName=""
if (len(sys.argv)>2):
	graphName=sys.argv[2]

def UnPack(file):
	res=[]
	i=0
	for line in file:
		res.insert(i,line.split(';'))
		res[i].pop()
		j=0
		for elem in res[i]:
			res[i][j]=float(elem)
			j=j+1
		i=i+1
	return res
	
dat = UnPack(file)

fig = plt.figure()
ax = fig.add_subplot(111)

cm='gray'
#cs = ax.contourf(dat, cmap=cm)
cs = ax.imshow(dat, cmap=cm)
fig.colorbar(cs, ax=ax)
ax.set_title(graphName)

plt.tight_layout()

if (graphName!=""):
	plt.savefig(filePath+graphName+'.png')

file.close()

if (len(sys.argv)<=3):	
	plt.show()







