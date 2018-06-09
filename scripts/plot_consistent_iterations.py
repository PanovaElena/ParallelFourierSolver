import subprocess;

N=30
i=0
name="C:\\Users\\Alyona\\Documents\\Visual Studio 2017\\Projects\\parallel-algorithms\\files\\field_solver_test_pulse_E\\iter_"
while(i<=N):
	name1=name+str(i)+".csv"
	subprocess.Popen("python graph_mat_plot_lib.py \""+ name1 + "\" " + str(i) + " no")
	i=i+1