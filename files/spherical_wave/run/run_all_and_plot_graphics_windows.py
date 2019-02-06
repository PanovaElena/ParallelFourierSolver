import sys
import math
sys.path.append("../../scripts/")
import subprocess
import os
import shutil
import graphics as gr
import args

DIR_SCRIPT = "./"+os.path.dirname(sys.argv[0])
FOLDER="/Release/"

NAME_SEQ_PROGRAM = "\""+DIR_SCRIPT+"/../../../build/src/Examples/SphericalWave/SequentialFieldSolver_SphericalWave/"+FOLDER+"/sphericalWave_sequential"+"\""
NAME_PAR_PROGRAM = "\""+DIR_SCRIPT+"/../../../build/src/Examples/SphericalWave/ParallelFieldSolver_SphericalWave/"+FOLDER+"/sphericalWave_parallel"+"\""

DIR_PICTURES = "./pictures/"
DIR_RESULTS = "./results/"

NAME_FILE_SEQ = DIR_RESULTS+"/sequential_result.csv"
NAME_FILE_PAR = DIR_RESULTS+"/parallel_result.csv"

if (os.path.exists(DIR_PICTURES)): shutil.rmtree(DIR_PICTURES)
os.mkdir(DIR_PICTURES)
if (os.path.exists(DIR_RESULTS)): shutil.rmtree(DIR_RESULTS)
os.mkdir(DIR_RESULTS)
		
funcRead = gr.readFile2d
funcPlot = gr.plot2d
funcPlotError = gr.plotError2d

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

data_seq = funcRead(NAME_FILE_SEQ)


# parallel

command_args_par = "-ax "+str(args.ax)+" "+\
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
					"-npx "+str(args.npx)+" "+\
					"-npy "+str(args.npy)+" "+\
					"-npz "+str(args.npz)+" "+\
					\
					"-gx "+str(args.gx)+" "+\
					"-gy "+str(args.gy)+" "+\
					"-gz "+str(args.gz)+" "+\
					\
					"-nseqi "+str(args.n_sequential_iter)+" "+\
					"-npari "+str(args.n_parallel_iter)+" "+\
					"-ndomi "+str(args.number_of_iterations_in_domain)+" "+\
					\
					"-scheme "+str(args.scheme)+" "+\
					\
					"-mask "+str(args.mask)+" "+\
					"-mwx "+str(args.mwx)+" "+\
					"-mwy "+str(args.mwy)+" "+\
					"-mwz "+str(args.mwz)+" "+\
					\
					"-filter "+str(args.filter)+" "+\
					"-fwx "+str(args.fwx)+" "+\
					"-fwy "+str(args.fwy)+" "+\
					"-fwz "+str(args.fwz)+" "+\
					"-fnzx "+str(args.fnzx)+" "+\
					"-fnzy "+str(args.fnzy)+" "+\
					"-fnzz "+str(args.fnzz)+" "+\
					\
					"-dim "+str(args.dimension_of_output_data)+" "+\
					\
					"-dir "+str(DIR_SCRIPT+DIR_RESULTS)+" "\
					\
					"-omega "+str(args.omega)+" "+\
					"-omenv "+str(args.omega_envelope)+" "+\
					"-ts "+str(args.time_source)+" "+\
					"-ws "+str(args.width_source)+" ";
					
np=args.npx*args.npy*args.npz
					
process_par = subprocess.Popen("mpiexec -n "+str(np)+" "+NAME_PAR_PROGRAM+" "+command_args_par, shell=True)
process_par.wait()

data_par = funcRead(NAME_FILE_PAR)


# plot

os.walk(DIR_RESULTS)
for (dirpath, dirnames, filenames) in os.walk(DIR_RESULTS):
	for file in filenames:
		funcPlot(DIR_PICTURES, file, funcRead(DIR_RESULTS+file))

funcPlotError(DIR_PICTURES, "error", data_seq, data_par)
	





