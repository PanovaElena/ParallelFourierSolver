#include "mpi_wrapper.h"
#include <string>
#include <iostream>
#include <omp.h>
#include "class_member_ptr.h"
#include "running_wave.h"
#include "parser_running_wave.h"
#include "status.h"
//#include "fftw3.h"
//#include "fftw3-mpi.h"
#include "grid3d.h"
#include <mkl_cdft.h>

static DFTI_DESCRIPTOR_DM_HANDLE desc;
static vec3<int> gs;

void useFFTW(Array3d<double>& arr1, Array3d<MyComplex>& arr2, int Nx, int Ny, int Nz,
    Direction dir) {
    //fftw_plan plan = 0;
    //fftw_plan_with_nthreads(omp_get_max_threads());
    switch (dir) {
    case RtoC:
        DftiComputeForwardDM(desc, &(arr1[0]), &(arr2[0]));
        /*plan = fftw_mpi_plan_dft_r2c_3d(gs.x, gs.y, gs.z, &(arr1[0]), (fftw_complex*)&(arr2[0]),
            MPI_COMM_WORLD, FFTW_ESTIMATE);
        fftw_execute(plan);*/
        break;
    case CtoR:
        DftiComputeBackwardDM(desc, &(arr2[0]), &(arr1[0]));
        /*plan = fftw_mpi_plan_dft_c2r_3d(gs.x, gs.y, gs.z, (fftw_complex*)&(arr2[0]), &(arr1[0]),
            MPI_COMM_WORLD, FFTW_ESTIMATE);
        fftw_execute(plan);*/
        for (int i = 0; i < arr1.size1d(); i++)
            arr1[i] /= gs.x*gs.y*gs.z;
        break;
    }
    //fftw_destroy_plan(plan);
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

    for (int j = 1; j <= runningWave.params.nSeqSteps; j++) {
        fourierTransform(runningWave.gr, RtoC);
        runningWave.params.fieldSolver(runningWave.gr, runningWave.params.dt);
        fourierTransform(runningWave.gr, CtoR);
    }

    double t2 = omp_get_wtime();
    MPIWrapper::showMessage("Time of mpi fft version is " +std::to_string(t2 - t1));

    runningWave.params.fileWriter.write(runningWave.gr, "mpi_fft_result_rank_"+
        std::to_string(MPIWrapper::MPIRank())+".csv", Double, "writing...");
}

int main(int argc, char** argv) {
    MPIWrapper::MPIInitialize(argc, argv);
    //fftw_mpi_init();
    //fftw_init_threads();

    ParametersForRunningWave params;
    ParserRunningWave parser;

    Status status = parser.parseArgsForParallel(argc, argv, params);

    if (status == Status::OK) {
        //if (MPIWrapper::MPIRank() == 0) params.print();

        // computing of local sizes
       /* ptrdiff_t localSizeX = -1, localStartX = -1;
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

        testBody(runningWave, params.n);*/



        int localSizeX = -1, localStartX = -1, v=-1;
        int len[3] = { params.n.x, params.n.y, params.n.z };

        long long s = DftiCreateDescriptorDM(MPI_COMM_WORLD, &desc, DFTI_DOUBLE, DFTI_REAL, 3, len);

        DftiGetValueDM(desc, CDFT_LOCAL_SIZE, &v);
        DftiGetValueDM(desc, CDFT_LOCAL_NX, &localSizeX);
        DftiGetValueDM(desc, CDFT_LOCAL_X_START, &localStartX);

        std::cout << "\n" << "RANK " << MPIWrapper::MPIRank() << "\n";
        std::cout << s << " " << localSizeX << " " << localStartX << " " << v << "\n";

        if (v != localSizeX * params.n.y * params.n.z) {
            MPIWrapper::showMessage("ERROR: v is different");
            return 1;
        }

        vec3<int> localGridSize(localSizeX, params.n.y, params.n.z);
        vec3<int> localGridStart(localStartX, 0, 0);
        vec3<> aLocal(params.a + (vec3<>)localGridStart*params.d),
            bLocal(aLocal + (vec3<>)localGridSize*params.d);

        ParametersForRunningWave localParams(params);
        localParams.a = aLocal; localParams.b = bLocal; localParams.n = localGridSize;
        localParams.print();

        // Set that we want out-of-place transform (default is DFTI_INPLACE)
        DftiSetValueDM(desc, DFTI_PLACEMENT, DFTI_NOT_INPLACE);

        // Commit descriptor
        DftiCommitDescriptorDM(desc);

        RunningWave runningWave;
        runningWave.setParamsForTest(localParams);
        gs = params.n;

        testBody(runningWave);

        DftiFreeDescriptorDM(&desc);



        //int len[2], v, n, s;
        //MyComplex *in, *out;
        //int nx = 16, ny = 16;

        //// Create descriptor for 2D FFT
        //len[0] = nx;
        //len[1] = ny;
        //DftiCreateDescriptorDM(MPI_COMM_WORLD, &desc, DFTI_DOUBLE, DFTI_COMPLEX, 2, len);
        //// Ask necessary length of in and out arrays and allocate memory
        //DftiGetValueDM(desc, CDFT_LOCAL_SIZE, &v);
        //in = (MyComplex*)malloc(v * sizeof(MyComplex));
        //out = (MyComplex*)malloc(v * sizeof(MyComplex));
        //// Fill local array with initial data. Current process performs n rows,
        //// 0 row of in corresponds to s row of virtual global array
        //DftiGetValueDM(desc, CDFT_LOCAL_NX, &n);
        //DftiGetValueDM(desc, CDFT_LOCAL_X_START, &s);
        //// Virtual global array globalIN is defined by function f as
        //// globalIN[i*ny+j]=f(i,j)
        //for (int i = 0; i < n; i++)
        //    for (int j = 0; j < ny; j++)
        //        in[i*ny + j] = params.startCond->fE({ i + s, j ,0}).x;
        //// Set that we want out-of-place transform (default is DFTI_INPLACE)
        //DftiSetValueDM(desc, DFTI_PLACEMENT, DFTI_NOT_INPLACE);

        //std::cout << v << " " << n << " " << s << std::endl;

        //// Commit descriptor, calculate FFT, free descriptor
        //DftiCommitDescriptorDM(desc);
        //MPIWrapper::showMessage("start");
        //DftiComputeForwardDM(desc, in, out);
        //MPIWrapper::showMessage("done");
        //MPIWrapper::MPIBarrier();
        //// Virtual global array globalOUT is defined by function g as
        //// globalOUT[i*ny+j]=g(i,j)
        //// Now out contains result of FFT. out[i*ny+j]=g(i+s,j)
        //DftiFreeDescriptorDM(&desc);
        //free(in);
        //free(out);
        //MPIWrapper::showMessage("end");

    }
    else if (status == Status::ERROR)
        std::cout << "There are some problems in args" << std::endl;

    MPIWrapper::MPIFinalize();
    return 0;
}