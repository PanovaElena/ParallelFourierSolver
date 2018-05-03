from tkinter import *
import math
from PIL import Image, ImageGrab
import sys
import time

#файл
file=open(sys.argv[1], 'r')

# глобальные переменные
# настройки окна
WINDOW_HEIGHT = 500

# ширина ячейки
CELL_SIZE = 0

# количество ячеек
NX = 0
NY = 0

# устанавливаем окно
root = Tk()
root.title("Graph")

#максимум и минимум цвета
MAX_NUM = -1000000000
MIN_NUM = 1000000000

def CalcNM(arrNum):
	global NX
	global NY
	NY=0
	NX=0
	for line in arrNum:
		NY=NY+1
	for elem in arrNum[0]:
		NX=NX+1
	
def CalcMaxMin(arrNum):
	global MAX_NUM
	global MIN_NUM
	for line in arrNum:
		for elem in line:
			if (float(elem) > MAX_NUM):
				MAX_NUM=float(elem)
			if (float(elem) < MIN_NUM):
				MIN_NUM=float(elem)	
	
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
	
def GetColor(intens):
	if (MAX_NUM==MIN_NUM):
		return "#000"
	col=int((float(intens)-MIN_NUM)/(MAX_NUM-MIN_NUM)*16)
	symb='0'
	if (col==10):
		symb='a'
	if (col==11):
		symb='b'
	if (col==12):
		symb='c'
	if (col==13):
		symb='d'
	if (col==14):
		symb='e'
	if (col==15):
		symb='f'
	str='#'+symb+symb+symb
	return str
	
arrNum = UnPack(file)
CalcNM(arrNum)
CELL_SIZE=WINDOW_HEIGHT/NY
CalcMaxMin(arrNum)

# область
canvas = Canvas(root, width=CELL_SIZE*NX, height=CELL_SIZE*NY, background="#ffffff")
canvas.pack()

j=0
for line in arrNum:
	i=0
	for elem in line:
		color=GetColor(elem)
		canvas.create_rectangle(i*CELL_SIZE, j*CELL_SIZE, (i+1)*CELL_SIZE, (j+1)*CELL_SIZE, 
            outline=color, fill=color)
		i=i+1
	j=j+1

file.close()

I=0
def Screen():
	global I
	
	if (I==3):
		# скринит экран с окном
		box = (canvas.winfo_rootx(),canvas.winfo_rooty(),canvas.winfo_rootx()+canvas.winfo_width(),canvas.winfo_rooty() + canvas.winfo_height())
		grab = ImageGrab.grab(bbox = box)
		grab.save(sys.argv[1][0:len(sys.argv[1])-4]+"_graph"+".png")
	
	I=I+1
	root.after(50, Screen)

Screen()

# запускаем работу окна
root.mainloop()
	
