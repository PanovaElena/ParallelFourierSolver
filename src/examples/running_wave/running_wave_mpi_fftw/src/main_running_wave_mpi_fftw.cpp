#include <string>
#include <iostream>
#include "mpi_wrapper.h"
#include "running_wave.h"
#include "parser_running_wave.h"
#include "status.h"
#include "fftw3.h"
#include "fftw3-mpi.h"
#include "grid3d.h"

void testBody(RunningWave& runningWave) {

    double t1 = omp_get_wtime();
    bool useMpi = true;

    transformGridIfNecessary(runningWave.params.fieldSolver, runningWave.gr, RtoC, useMpi);
    for (int j = 1; j <= runningWave.params.nSeqSteps; j++) {
        runningWave.params.fieldSolver(runningWave.gr, runningWave.params.dt);
    }

    transformGridIfNecessary(runningWave.params.fieldSolver, runningWave.gr, CtoR, useMpi);

    double t2 = omp_get_wtime();
    std::cout << "Time of mpi fft version is " << t2 - t1 << std::endl;

    runningWave.params.fileWriter.write(runningWave.gr, "mpi_fft_result_rank_"+
        std::to_string(MPIWrapper::MPIRank())+".csv", Double, "writing...");
}

int main(int argc, char** argv) {
    MPIWrapper::MPIInitialize(argc, argv);
    fftw_mpi_init();
    fftw_init_threads();

    ParametersForRunningWave params;
    ParserRunningWave parser;

    Status status = parser.parseArgsForParallel(argc, argv, params);

    if (status == Status::OK) {
        if (MPIWrapper::MPIRank() == 0) params.print();

        // computing of local sizes
        ptrdiff_t localSizeX, localStartX;
        fftw_mpi_local_size_3d(params.n.x, params.n.y, params.n.z / 2 + 1, MPI_COMM_WORLD,
            &localSizeX, &localStartX);
        vec3<int> localGridSize(localSizeX, params.n.y, 2 * (params.n.z / 2 + 1));
        vec3<int> localGridStart(localStartX, 0, 0);
        vec3<> aLocal(params.a + (vec3<>)localGridStart*params.d),
            bLocal(aLocal + (vec3<>)localGridSize*params.d);

        ParametersForRunningWave localParams(params);
        localParams.a = aLocal; localParams.b = bLocal; localParams.n = localGridSize;
        std::cout << "\n" << "RANK " << MPIWrapper::MPIRank() << "\n";
        localParams.print();

        RunningWave runningWave;
        runningWave.setParamsForTest(localParams);

    }
    else if (status == Status::ERROR)
        std::cout << "There are some problems in args" << std::endl;

    MPIWrapper::MPIFinalize();
}