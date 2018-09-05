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

const std::string nameFileStartParallel = dir + "SecondStepsStartE";
const std::string nameFileFinalParallel = dir + "SecondStepsFinalE";

std::string arrNameFileFinalSecondSteps[100];
std::string arrNameFileStartSecondSteps[100];
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

void InitNamesFiles() {
    for (int i = 0; i < MPIWrapper::MPISize(); i++) {
        arrNameFileFinalSecondSteps[i] = nameFileFinalParallel + std::to_string(i) + ".csv";
        arrNameFileStartSecondSteps[i] = nameFileStartParallel + std::to_string(i) + ".csv";
    }
}

int main(int* argc, char** argv) {
    MPIWrapper::MPIInitialize();
    InitNamesFiles();
    TestBody();
    MPIWrapper::MPIFinalize();
}