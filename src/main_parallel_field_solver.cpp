#include "field_solver.h"
#include "pulse.h"
#include "simple_types.h"
#include <fstream>
#include "mpi_worker.h"
#include "mpi_wrapper.h"

const std::string dir = "../../../files/parallel_field_solver_console/";
const std::string nameFileFirstSteps = dir + "FirstStepsE.csv";
const std::string nameFileSecondSteps = dir + "SecondStepsE.csv";
const std::string nameFileSecondStepsLeft = dir + "SecondStepsELeft.csv";
const std::string nameFileSecondStepsRight = dir + "SecondStepsERight.csv";
const std::string arrNameFileSecondSteps[] = { nameFileSecondStepsLeft, nameFileSecondStepsRight };

const int NStartSteps = 20;
const int NNextSteps = 8;

void WriteFile(Grid3d& gr, std::string nameFile) {
    std::ofstream file;
    file.open(nameFile);

    for (int j = 0; j < gr.gnyRealNodes(); j++) {
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            file << gr(i, j, gr.gnzRealCells() / 2).E.getNorm() << ";";
        file << std::endl;
    }

    file.close();
}

void DoFirstPart(Pulse& pulse) {
    for (int i = 1; i < NStartSteps; i++) {
        pulse.SetJ(i);
        FieldSolverPSATD(pulse.gr, pulse.dt);
    }

    FourierTransformation(pulse.gr, CtoR);
    if (MPIWrapper::MPIRank() == 0)
        WriteFile(pulse.gr, nameFileFirstSteps);
}

void DoSecondPart(Pulse& pulse) {
    MPIWorker worker(pulse.gr, pulse.gr.gnxRealCells() / 8);

    FieldSolverParallelPSATD(worker, NNextSteps, pulse.dt);

    WriteFile(worker.getGrid(), arrNameFileSecondSteps[MPIWrapper::MPIRank()]);

    worker.AssembleResultsToZeroProcess(pulse.gr);
    if (MPIWrapper::MPIRank()==0)
        WriteFile(pulse.gr, nameFileSecondSteps);
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