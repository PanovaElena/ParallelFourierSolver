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

static std::string arrNameFileFinalSecondSteps[100];
static std::string arrNameFileStartSecondSteps[100];
const std::string consistentResult = "../../../files/field_solver_test_pulse_E/iter_40.csv";

void DoConsistentPart(Pulse& pulse) {
    MPIWorker::ShowMessage("do first steps");
    for (int i = 1; i <= Pulse::NStartSteps; i++) {
        pulse.SetJ(i);
        FieldSolverPSATD(pulse.gr, pulse.dt);
    }

    FourierTransformation(pulse.gr, CtoR);
    /*MPIWorker::ShowMessage("write to file first steps");
    if (MPIWrapper::MPIRank() == 0)
        WriteFileFieldAbs(pulse.gr, nameFileFirstSteps);*/
}

void DoParallelPart(Pulse& pulse) {
    MPIWorker worker(pulse.gr, pulse.gr.gnxRealCells()/4); //128, 16
    MPIWorker::ShowMessage("write to file first domain");
    WriteFileField(E, z, worker.getGrid(), arrNameFileStartSecondSteps[MPIWrapper::MPIRank()]);

    MPIWorker::ShowMessage("parallel field solver");
    FieldSolverParallelPSATD(worker, Pulse::NNextSteps, pulse.dt, NNextSteps, dir);

    MPIWorker::ShowMessage("write to file parallel result");
    WriteFileField(E, z, worker.getGrid(), arrNameFileFinalSecondSteps[MPIWrapper::MPIRank()]);

    MPIWorker::ShowMessage("assemble");
    worker.AssembleResultsToZeroProcess(pulse.gr);
    MPIWorker::ShowMessage("write to file assembled result");
    if (MPIWrapper::MPIRank()==0)
        WriteFileField(E, z, pulse.gr, nameFileSecondSteps);
}

void TestBody() {
    Pulse pulse;
    DoConsistentPart(pulse);
    DoParallelPart(pulse);
}

void InitFiles() {
    for (int i = 0; i < MPIWrapper::MPISize(); i++) {
        arrNameFileFinalSecondSteps[i] = nameFileFinalParallel + std::to_string(i) + ".csv";
        arrNameFileStartSecondSteps[i] = nameFileStartParallel + std::to_string(i) + ".csv";
    }
}

int main(int* argc, char** argv) {
    MPIWrapper::MPIInitialize();
    InitFiles();
    TestBody();
    MPIWrapper::MPIFinalize();
}