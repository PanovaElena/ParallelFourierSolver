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

const std::string nameFileStartParallelLeft = dir + "SecondStepsStartELeft.csv";
const std::string nameFileFinalParallelLeft = dir + "SecondStepsFinalELeft.csv";
const std::string nameFileStartParallelRight = dir + "SecondStepsStartERight.csv";
const std::string nameFileFinalParallelRight = dir + "SecondStepsFinalERight.csv";

const std::string arrNameFileFinalSecondSteps[] = { nameFileFinalParallelLeft, nameFileFinalParallelRight };
const std::string arrNameFileStartSecondSteps[] = { nameFileStartParallelLeft, nameFileStartParallelRight };
const std::string consistentResult = "../../../files/field_solver_test_pulse_E/iter_40.csv";

const int NStartSteps = 31;
const int NNextSteps = 10;

void DoConsistentPart(Pulse& pulse) {
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

void DoParallelPart(Pulse& pulse) {
    MPIWorker worker(pulse.gr, pulse.gr.gnxRealCells()/8); //128, 16
    MPIWorker::ShowMessage("write to file first domain");
    WriteFileE(worker.getGrid(), arrNameFileStartSecondSteps[MPIWrapper::MPIRank()]);

    MPIWorker::ShowMessage("parallel field solver");
    FieldSolverParallelPSATD(worker, NNextSteps, pulse.dt, NNextSteps, dir);

    MPIWorker::ShowMessage("write to file parallel result");
    WriteFileE(worker.getGrid(), arrNameFileFinalSecondSteps[MPIWrapper::MPIRank()]);

    MPIWorker::ShowMessage("assemble");
    worker.AssembleResultsToZeroProcess(pulse.gr);
    MPIWorker::ShowMessage("write to file assembled result");
    if (MPIWrapper::MPIRank()==0)
        WriteFileE(pulse.gr, nameFileSecondSteps);
}

void TestBody() {
    Pulse pulse;
    DoConsistentPart(pulse);
    DoParallelPart(pulse);
}

int main(int* argc, char** argv) {
    MPIWrapper::MPIInitialize();
    TestBody();
    MPIWrapper::MPIFinalize();
}