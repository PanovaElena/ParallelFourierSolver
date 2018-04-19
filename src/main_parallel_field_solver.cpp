#include "field_solver.h"
#include "pulse.h"
#include "simple_types.h"
#include <fstream>
#include "mpi_worker.h"
#include "mpi_wrapper.h"

const std::string nameFile40Step = "../../files/parallel_field_solver_console/40StepE.csv";
const std::string nameFile50Step = "../../files/parallel_field_solver_console/50StepE.csv";
const std::string nameFileLeft50Step = "../../files/parallel_field_solver_console/LeftE50Step.csv";
const std::string nameFileRight50Step = "../../files/parallel_field_solver_console/RightE50Step.csv";
const std::string arrNameFile50Step[] = { nameFileLeft50Step, nameFileRight50Step };

const int NStartSteps = 20;
const int NNextSteps = 8;

void WriteFile(Grid3d& gr, std::string nameFile) {
    std::ofstream file;
    file.open(nameFile);

    for (int i = 0; i < gr.gnxRealNodes(); i++) {
        for (int j = 0; j < gr.gnyRealNodes(); j++)
            file << gr(i, j, gr.gnzRealCells() / 2).E.getNorm() << ";";
        file << std::endl;
    }

    file.close();
}

void TestBody() {
    Pulse pulse;
    MPIWorker worker(pulse.gr, pulse.gr.gnxRealCells() / 5);

    for (int i = 0; i < NStartSteps; i++) {
        pulse.SetJ(i);
        FieldSolverPSATD(pulse.gr, pulse.dt);
    }

    WriteFile(pulse.gr, nameFile40Step);

    FieldSolverParallelPSATD(worker, NNextSteps, pulse.dt);

    WriteFile(worker.getGrid(), arrNameFile50Step[MPIWrapper::MPIRank()]);

}

int main(int* argc, char** argv) {
    MPIWrapper::MPIInitialize();
    TestBody();
    MPIWrapper::MPIFinalize();
}