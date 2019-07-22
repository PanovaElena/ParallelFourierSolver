import sys
import math
sys.path.append("../../scripts/")
import read_file as fr
import graphics as gr
import subprocess
import os
import shutil
import args

DIR_SCRIPT = "./"+os.path.dirname(sys.argv[0])
if (sys.platform == "win32"):
	FOLDER="/Release/"
else: FOLDER=""

NUM_PROCESSES=2

MPI="mpiexec"

NAME_PROGRAM = "\""+DIR_SCRIPT+"/../../../build/src/examples/running_wave/running_wave_mpi_fftw/"+FOLDER+"/running_wave_mpi_fftw"+"\""

DIR_PICTURES = "./pictures/"
DIR_RESULTS = "./results/"

if (os.path.exists(DIR_PICTURES)): 
	for (dirpath, dirnames, filenames) in os.walk(DIR_PICTURES):
		for file in filenames:
			os.remove(DIR_PICTURES+file)
else: os.mkdir(DIR_PICTURES)

if (os.path.exists(DIR_RESULTS)): 
	for (dirpath, dirnames, filenames) in os.walk(DIR_RESULTS):
		for file in filenames:
			os.remove(DIR_RESULTS+file)
else: os.mkdir(DIR_RESULTS)
		
if (args.dimension_of_output_data==1):
	funcRead = fr.readFile1d
	funcPlot = gr.plot1d
else: 
	funcRead = fr.readFile2d
	funcPlot = gr.plot2d

command_args = "-ax "+str(args.ax)+" "+\
					"-ay "+str(args.ay)+" "+\
					"-az "+str(args.az)+" "+\
					\
					"-dx "+str(args.dx)+" "+\
					"-dy "+str(args.dy)+" "+\
					"-dz "+str(args.dz)+" "+\
					\
					"-nx "+str(args.nx)+" "+\
					"-ny "+str(args.ny)+" "+\
					"-nz "+str(args.nz)+" "+\
					\
					"-dt "+str(args.dt)+" "+\
					\
					"-solver "+str(args.solver)+" "+\
					\
					"-nseqi "+str(args.n_iter)+" "+\
					\
					"-dim "+str(args.dimension_of_output_data)+" "\
					\
					"-dir "+str(DIR_SCRIPT+DIR_RESULTS)+" "\
					\
					"-lambda "+str(args.lambd)+" "+\
					"-angle "+str(args.angle)+" ";
					

process = subprocess.Popen(MPI+" -n "+str(NUM_PROCESSES)+" "+NAME_PROGRAM+" "+command_args, shell=True)
process.wait()


os.walk(DIR_RESULTS)
for (dirpath, dirnames, filenames) in os.walk(DIR_RESULTS):
	for file in filenames:
		funcPlot(DIR_PICTURES, file, funcRead(DIR_RESULTS+file))
	





