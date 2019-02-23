To build use cmake. Environment variables I_MPI_ROOT, INTEL, MKLROOT are to be defined.
Use "-G <VS Generator> -T <Platform Toolset>" in cmake command line to build on Windows
(for ex. -G "Visual Studio 15 2017 Win64" -T "Intel C++ Compiler 18.0"). 

For easy launch use ./files/[example]/run/run_all_and_plot_graphics_windows.py.
Command arguments can be set in ./files/[example]/run/args.py.

Executable files can be found in directories:
build/src/examples/running_wave/running_wave_sequential,
build/src/examples/running_wave/running_wave_parallel,
build/src/examples/spherical_wave/spherical_wave_sequential,
build/src/examples/spherical_wave/spherical_wave_parallel.

To get information about command arguments use option --help.
