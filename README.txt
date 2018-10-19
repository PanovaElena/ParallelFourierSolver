To build the project you should use cmake. If you OS is Windows, move file 
lib/fftw-3.3.5_windows_x64/libfftw3-3.dll to directories with executable files.
The executable files with examples will be in directories:
build/src/Examples/RunningWave/ConsistentFieldSolver_RunningWave
build/src/Examples/RunningWave/ParallelFieldSolver_RunningWave
build/src/Examples/SphericalWave/ConsistentFieldSolver_SphericalWave
ect
To get information about command arguments use option --help.
Results of execution will be in the next directories:
files/running_wave/consistent_results
files/running_wave/parallel_results
files/spherical_wave/consistent_results
ect
To plot graphics use script plot_all_graphics_for_[example].py, which gets number of processes as a command arg.