def computeError1d(data1, data2):
	data=[]
	for i in range(len(data1)):
		data.append(data1[i]-data2[i])
	return data
	
def computeError2d(data1, data2):
	data=[]
	for i in range(len(data1)):
		tmp=[]
		for j in range(len(data1[i])):
			tmp.append(data1[i][j]-data2[i][j])
		data.append(tmp)
	return data
