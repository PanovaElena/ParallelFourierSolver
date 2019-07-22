To build use cmake. Environment variables I_MPI_ROOT, INTEL, MKLROOT are to be defined.
Use "-G <VS Generator> -T <Platform Toolset>" in cmake command line to build on Windows
(for ex. -G "Visual Studio 15 2017 Win64" -T "Intel C++ Compiler 18.0"). 

For easy launch use ./files/[example]/run/run_all_and_plot_graphics_windows.py.
Command arguments can be set in ./files/[example]/run/args.py.

Executable files can be found in directories:
build/src/examples/running_wave/running_wave_sequential,
build/src/examples/running_wave/running_wave_parallel,
build/src/examples/spherical_wave/spherical_wave_sequential,
build/src/examples/spherical_wave/spherical_wave_parallel,
and so on.

To get information about command arguments use option --help.

To use mkl fft with mpi create a wrapper library:
1) go to "mkl\interfaces\fftw3x_cdft" and open the command line
2) run makefile with make (for linux) or nmake (for windows, before should run VsDevCmd.bat), command line is such as:
nmake libintel64 compiler="C:\Program Files (x86)\IntelSWTools\compilers_and_libraries\windows\bin\intel64\icl.exe" mpi=intelmpi INSTALL_DIR="C:\Users\Elena\Documents\Visual Studio 2017\Projects\plasma\parallel-algorithms\build\3rdparty\mkl_wrapper\"
3) link generated fftw3x_cdft.lib with project running_wave_mpi_fftw

