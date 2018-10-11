#include "mpi_worker.h"
#include "field_solver.h"
#include "fourier_transformation.h"
#include <fstream>
#include "file_writer.h"
#include "simple_types_and_constants.h"
#include <algorithm>
#include <limits>

void WriteFile(Grid3d& gr, int iter, int rank, FileWriter& fileWriter) {
    FourierTransformation(gr, CtoR);
    //MPIWorker::ShowMessage("writing parallel result to file, iter=" + std::to_string(iter));
    fileWriter.WriteFile(gr, "iter_rank_" + std::to_string(rank) + ".csv");
}

void FieldSolverParallelInnerCircle(MPIWorker& worker, FieldSolverType fieldSolver, int numIter, double dt,
    int iterWriteFile, FileWriter& fileWriter, int I) {
    FourierTransformation(worker.getGrid(), RtoC); //это нужно
    for (int i = 0; i < numIter; i++) {
        fieldSolver(worker.getGrid(), dt);
        if (iterWriteFile!=0 && (i + 1) % iterWriteFile == 0)
            WriteFile(worker.getGrid(), i, MPIWrapper::MPIRank(), fileWriter);
    }
    FourierTransformation(worker.getGrid(), CtoR);
    worker.ExchangeGuard();
}

vec3<double> round(vec3<double> a, int signAfterDot) {
    double p = pow(10, signAfterDot);
    return (vec3<double>((int)(a.x*p), (int)(a.y*p), (int)(a.z*p)) + 0.5) / p;
}

int CalcMaxInnerIter(MPIWorker& worker, double dt) {
    vec3<int> tmpVec = static_cast<vec3<int>>(round(0.75*worker.getGrid().gd()*worker.getGuardSize() / (constants::c*dt), 6));
    if (tmpVec.x == 0 && worker.getSize().x == 1) tmpVec.x = std::numeric_limits<int>::max();
    if (tmpVec.y == 0 && worker.getSize().y == 1) tmpVec.y = std::numeric_limits<int>::max();
    if (tmpVec.z == 0 && worker.getSize().z == 1) tmpVec.z = std::numeric_limits<int>::max();
    return std::min(tmpVec.x, std::min(tmpVec.y, tmpVec.z));
}

void FieldSolverParallel(MPIWorker& worker, FieldSolverType fieldSolver, int numIter, double dt,
    int iterWriteFile, FileWriter& fileWriter) {
    int maxInnerIter = CalcMaxInnerIter(worker, dt);
    int numOuterIter = numIter / maxInnerIter;
    int numRemaindIter = numIter % maxInnerIter;
    if (MPIWrapper::MPIRank() == 0) {
        worker.ShowMessage("numOuterIter " + std::to_string(numOuterIter) + " inner circles");
        worker.ShowMessage("maxInnerIter " + std::to_string(maxInnerIter));
        worker.ShowMessage("numRemaindIter " + std::to_string(numRemaindIter));
    }

    for (int i = 0; i < numOuterIter; i++) {
        //worker.ShowMessage(std::to_string(i+1)+" inner circle");
        FieldSolverParallelInnerCircle(worker, fieldSolver, maxInnerIter, dt, 0, fileWriter, i+1);
        worker.ApplyMask();
    }
    FieldSolverParallelInnerCircle(worker, fieldSolver, numRemaindIter, dt, numRemaindIter, fileWriter, 0);
}