#include "field_solver.h"
#include "pulse.h"
#include "simple_types.h"
#include <fstream>
#include "mpi_worker.h"
#include "mpi_wrapper.h"
#include "write_file.h"

const std::string dir = "../../../files/parallel_field_solver_console/";
const std::string nameFileFirstSteps = dir + "FirstStepsE.csv";
const std::string nameFileSecondSteps = dir + "SecondStepsE.csv";
const std::string nameFileSecondStepsLeft = dir + "SecondStepsELeft.csv";
const std::string nameFileSecondStepsRight = dir + "SecondStepsERight.csv";
const std::string arrNameFileSecondSteps[] = { nameFileSecondStepsLeft, nameFileSecondStepsRight };

const int NStartSteps = 30;
const int NNextSteps = 10;

void DoFirstPart(Pulse& pulse) {
    MPIWorker::ShowMessage("do first steps");
    for (int i = 1; i < NStartSteps; i++) {
        pulse.SetJ(i);
        FieldSolverPSATD(pulse.gr, pulse.dt);
    }

    MPIWorker::ShowMessage("write to file first steps");
    FourierTransformation(pulse.gr, CtoR);
    if (MPIWrapper::MPIRank() == 0)
        WriteFileE(pulse.gr, nameFileFirstSteps);
}

void DoSecondPart(Pulse& pulse) {
    MPIWorker worker(pulse.gr, pulse.gr.gnxRealCells() / 8);

    MPIWorker::ShowMessage("parallel field solver");
    FieldSolverParallelPSATD(worker, NNextSteps, pulse.dt, NNextSteps, dir);

    MPIWorker::ShowMessage("write to file parallel result");
    WriteFileE(worker.getGrid(), arrNameFileSecondSteps[MPIWrapper::MPIRank()]);

    MPIWorker::ShowMessage("assemble");
    worker.AssembleResultsToZeroProcess(pulse.gr);
    MPIWorker::ShowMessage("write to file assembled result");
    if (MPIWrapper::MPIRank()==0)
        WriteFileE(pulse.gr, nameFileSecondSteps);
}

void TestBody() {
    Pulse pulse;
    DoFirstPart(pulse);
    DoSecondPart(pulse);
}

int main(int* argc, char** argv) {
    MPIWrapper::MPIInitialize();
    TestBody();
    MPIWrapper::MPIFinalize();
}