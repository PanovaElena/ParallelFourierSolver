#pragma once
#include <map>
#include "command_line_parser.h"
#include "running_wave.h"
#include "masks.h"
#include "field_solver.h"

class ParserRunningWave : public CommandLineParser {
    const int nTDef = 1, nIterDef = 4;
public:
    void help(Task task) override {
        ParametersForRunningWave p;
        std::cout <<
            "--help                    get help\n" <<
            "--nT, --nIter             set number of periods and number of iterations in a period, default values are " << nTDef << " and " << nIterDef << ", \"nCons\" equates to 0\n" <<
            "--dt                      set time step (less important then \"--nT\" and \"--nIter\"), default value is " << p.dt << "\n" <<
            "--lambda                  set length of wave (number of cells), default value is " << int(p.lambda / p.d + 0.5) << "\n" <<
            "--nx, --ny, --nz          set size of grid, default value is " << p.nx << ", " << p.ny << ", " << p.nz << "\n" <<
            "--guard                   set width of guard, default value is " << p.guard << "\n" <<
            "-d                        set grid spacing, default value is " << p.d << "\n" <<
            "--mask                    set mask (\"simple\" or \"smooth\"), default value is \"smooth\"\n" <<
            "--solver                  set solver (\"PSTD\" or \"PSATD\"), default value is \"PSATD\"\n" <<
            "--nbd                     set number of iterations between dumps, default value is " << p.nIterBetweenDumps << "\n" <<
            "--nCons                   set number of consistent steps, default value is " << p.nConsSteps << "\n";
        if (task == Task::parallel) {
            std::cout <<
                "--nPar                    set number of parallel steps, less important then \"--nT\" and \"--nIter\", default value is " << p.nParSteps << "\n" <<
                "--npx, --npy, --npz       set num of domains for every dimension, sqrt3(MPISize)\n";
        }
        std::cout << std::endl;
    }

    int saveArgs(ParametersForTest& p, Task task) override {
        ParametersForRunningWave& params = static_cast<ParametersForRunningWave&>(p);
        int npx = mpiWrapper.MPISize().x, npy = mpiWrapper.MPISize().y, npz = mpiWrapper.MPISize().z;
        bool fnT = false, fnIter = false;
        int nIter = nIterDef, nT = nTDef;
        for (auto it = m.begin(); it != m.end(); it++) {
            if (it->first == "--lambda") params.lambda = std::stoi(it->second)*params.d;
            if (it->first == "--nx") params.nx = std::stoi(it->second);
            if (it->first == "--ny") params.ny = std::stoi(it->second);
            if (it->first == "--nz") params.nz = std::stoi(it->second);
            if (it->first == "--guard") params.guard = std::stoi(it->second);
            if (it->first == "-d") params.d = std::stod(it->second);
            if (it->first == "--mask") {
                if (it->second == "simple")
                    params.mask = simpleMask;
                else params.mask = maskSineSquare;
            }
            if (it->first == "--solver") {
                if (it->second == "PSTD")
                    params.fieldSolver = FieldSolverPSTD;
                else params.fieldSolver = FieldSolverPSATD;
            }
            if (it->first == "--nbd") params.nIterBetweenDumps = std::stoi(it->second);
            if (it->first == "--dt") params.dt = std::stod(it->second);
            if (it->first == "--nCons") params.nConsSteps = std::stoi(it->second);
            if (it->first == "--nT") {
                nT = std::stoi(it->second);
                fnT = true;
            }
            if (it->first == "--nIter") {
                nIter = std::stoi(it->second);
                fnIter = true;
            }
            if (fnT||fnIter) {
                double T = params.lambda / constants::c;
                params.dt = T / (double) nIter;
                params.nConsSteps = 0;
                params.nParSteps = nT*nIter;
            }
            if (task == Task::parallel) {
                if (it->first == "--nPar") params.nParSteps = std::stoi(it->second);
                if (it->first == "--npx") npx = std::stoi(it->second);
                if (it->first == "--npy") npy = std::stoi(it->second);
                if (it->first == "--npz") npz = std::stoi(it->second);
            }

        }
        if (task == Task::parallel) {
            return mpiWrapper.CheckAndSetSize(npx, npy, npz);
        }
        return 0;
    }
};