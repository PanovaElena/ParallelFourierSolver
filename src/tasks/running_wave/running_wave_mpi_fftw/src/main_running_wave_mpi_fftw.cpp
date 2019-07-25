#include "mpi_wrapper.h"
#include <string>
#include <iostream>
#include <omp.h>
#include "class_member_ptr.h"
#include "running_wave.h"
#include "parser_running_wave.h"
#include "status.h"
#include "fftw3.h"
#include "fftw3-mpi.h"
#include "grid3d.h"

void useFFTW(Array3d<double>& arr1, Array3d<MyComplex>& arr2, int Nx, int Ny, int Nz,
    Direction dir) {
    fftw_plan plan = 0;
    fftw_plan_with_nthreads(omp_get_max_threads());
    switch (dir) {
    case RtoC:
        plan = fftw_mpi_plan_dft_r2c_3d(Nx, Ny, Nz, &(arr1[0]), (fftw_complex*)&(arr2[0]),
            MPI_COMM_WORLD, FFTW_ESTIMATE);
        fftw_execute(plan);
        break;
    case CtoR:
        plan = fftw_mpi_plan_dft_c2r_3d(Nx, Ny, Nz, (fftw_complex*)&(arr2[0]), &(arr1[0]),
            MPI_COMM_WORLD, FFTW_ESTIMATE);
        fftw_execute(plan);
        for (int i = 0; i < Nx * Ny * Nz; i++)
            arr1[i] /= Nx * Ny * Nz;
        break;
    }
    fftw_destroy_plan(plan);
}

void fourierTransform(Grid3d & gr, Field _field, Coordinate _coord, Direction dir) {
    Array3d<double>& arrD = (gr.*getMemberPtrField<double>(_field)).*getMemberPtrFieldCoord<double>(_coord);
    Array3d<MyComplex>& arrC = (gr.*getMemberPtrField<MyComplex>(_field)).*getMemberPtrFieldCoord<MyComplex>(_coord);

    useFFTW(arrD, arrC, gr.sizeReal().x, gr.sizeReal().y, gr.sizeReal().z, dir);
}

void fourierTransform(Grid3d & gr, Direction dir) {
    if (gr.getLastFourierTransformDirect() == dir) {
        std::cout << "Try to transform to the same direction: " << dir << std::endl;
        return;
    }

    fourierTransform(gr, E, x, dir);
    fourierTransform(gr, E, y, dir);
    fourierTransform(gr, E, z, dir);
    fourierTransform(gr, B, x, dir);
    fourierTransform(gr, B, y, dir);
    fourierTransform(gr, B, z, dir);
    fourierTransform(gr, J, x, dir);
    fourierTransform(gr, J, y, dir);
    fourierTransform(gr, J, z, dir);

    gr.setLastFourierTransformDirect(dir);
}

void testBody(RunningWave& runningWave) {

    double t1 = omp_get_wtime();

    fourierTransform(runningWave.gr, RtoC);
    for (int j = 1; j <= runningWave.params.nSeqSteps; j++) {
        runningWave.params.fieldSolver(runningWave.gr, runningWave.params.dt);
    }

    fourierTransform(runningWave.gr, CtoR);

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
        ptrdiff_t localSizeX = -1, localStartX = -1;
        ptrdiff_t ret = fftw_mpi_local_size_3d(params.n.x, params.n.y, params.n.z / 2 + 1, MPI_COMM_WORLD,
            &localSizeX, &localStartX);
        if (ret == 0) {
            if (MPIWrapper::MPIRank() == 0)
                std::cout << "ERROR: can't create mkl descriptor" << std::endl;
            MPIWrapper::MPIFinalize();
            return 0;
        }
        vec3<int> localGridSize(localSizeX, params.n.y, 2 * (params.n.z / 2 + 1));
        vec3<int> localGridStart(localStartX, 0, 0);
        vec3<> aLocal(params.a + (vec3<>)localGridStart*params.d),
            bLocal(aLocal + (vec3<>)localGridSize*params.d);
        ParametersForRunningWave localParams(params);
        localParams.a = aLocal; localParams.b = bLocal; localParams.n = localGridSize;
        std::cout << "\n" << "RANK " << MPIWrapper::MPIRank() << "\n";
        std::cout << localSizeX << " " << localStartX << " " << ret << "\n";
        localParams.print();

        RunningWave runningWave;
        runningWave.setParamsForTest(localParams);

        testBody(runningWave);

    }
    else if (status == Status::ERROR)
        std::cout << "There are some problems in args" << std::endl;

    MPIWrapper::MPIFinalize();
    return 0;
}