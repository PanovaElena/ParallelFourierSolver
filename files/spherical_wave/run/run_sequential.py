import sys
import math
sys.path.append("../../scripts/")
import read_file as rf
import subprocess
import os
import shutil
import args

DIR_SCRIPT = "./"+os.path.dirname(sys.argv[0])
if (sys.platform == "win32"):
	FOLDER="/Release/"
else: FOLDER=""

NAME_SEQ_PROGRAM = "\""+DIR_SCRIPT+"/../../../build/src/examples/spherical_wave/spherical_wave_sequential/"+FOLDER+"/spherical_wave_sequential"+"\""

DIR_RESULTS = "./results/"

NAME_FILE_SEQ = DIR_RESULTS+"/sequential_result.csv"

if (os.path.exists(DIR_RESULTS)): 
	for (dirpath, dirnames, filenames) in os.walk(DIR_RESULTS):
		for file in filenames:
			os.remove(DIR_RESULTS+file)
else: os.mkdir(DIR_RESULTS)
		
funcRead = rf.readFile2d

# sequential

command_args_seq = "-ax "+str(args.ax)+" "+\
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
					"-omega "+str(args.omega)+" "+\
					"-omenv "+str(args.omega_envelope)+" "+\
					"-ts "+str(args.time_source)+" "+\
					"-ws "+str(args.width_source)+" ";
					
					
process_seq = subprocess.Popen(NAME_SEQ_PROGRAM+" "+command_args_seq, shell=True)
process_seq.wait()
	





