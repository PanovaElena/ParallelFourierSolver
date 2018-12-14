#pragma once
#include <map>
#include "command_line_parser.h"
#include "running_wave.h"
#include "mask.h"
#include "filter.h"
#include "field_solver.h"

class ParserRunningWave : public CommandLineParser {
public:
    void help(Task task) override {
        ParametersForRunningWave p;
        std::cout <<
            "-help                    get help\n" <<
            "-dt                      set time step (less important then \"-nT\" and \"-nIter\"), default value is " << p.dt << "\n" <<
            "-lambdaN                 set length of wave (number of cells), default value is " << int(p.lambda / p.d + 0.5) << "\n" <<
            "-lambda                  set length of wave (more important, then \"-lambdaN\"), default value is " << p.lambda << "\n" <<
            "-angle                   set angle in degrees between axis OX and the dimension of wave, default value is " << p.angle << "\n" <<
            "-dim                     set dimension of output data (1 or 2), default value is " << p.dimensionOfOutputData << "\n" <<
            "-nx, -ny, -nz            set size of grid, default value is " << p.nx << ", " << p.ny << ", " << p.nz << "\n" <<
            "-d                       set grid spacing, default value is " << p.d << "\n" <<
            "-solver                  set solver (\"PSTD\", \"PSATD\", \"FDTD\" or \"PSATD_omp\"), default value is \"PSATD\"\n" <<
            "-nbd                     set number of iterations between dumps, default value is " << p.nIterBetweenDumps << "\n" <<
            "-nCons                   set number of consistent steps, default value is " << p.nConsSteps << "\n";
        if (task == Task::parallel) {
            std::cout <<
                "-nPar                    set number of parallel steps, less important then \"-nT\" and \"-nIter\", default value is " << p.nParSteps << "\n" <<
                "-guard                   set width of guard, default value is " << p.guard << "\n" <<
                "-mask                    set mask (\"simple\" or \"smooth\"), default value is \"smooth\"\n" <<
                "-filter                  set on or off for low-frequency filter (\"on\" or \"off\"), default value is " << p.filter.to_string() << "\n" <<
                "-npx, -npy, -npz       set num of domains for every dimension, default value is sqrt3(MPISize)" << std::endl;
        }
        std::cout << std::endl;
    }

    Status saveArgs(ParametersForTest& p, Task task) override {

        ParametersForRunningWave& params = static_cast<ParametersForRunningWave&>(p);

        if (m.find("-d") != m.end()) params.d = std::stod(m.find("-d")->second);
        if (m.find("-nx") != m.end()) params.nx = std::stoi(m.find("-nx")->second);
        if (m.find("-ny") != m.end()) params.ny = std::stoi(m.find("-ny")->second);
        if (m.find("-nz") != m.end()) params.nz = std::stoi(m.find("-nz")->second);
        if (m.find("-dt") != m.end()) params.dt = std::stod(m.find("-dt")->second);
        if (m.find("-lambdaN") != m.end()) params.lambda = std::stoi(m.find("-lambdaN")->second)*params.d;
        if (m.find("-lambda") != m.end()) params.lambda = std::stod(m.find("-lambda")->second);
        if (m.find("-angle") != m.end()) params.angle = constants::pi*std::stod(m.find("-angle")->second) / 180;
        if (m.find("-dim") != m.end()) params.dimensionOfOutputData = std::stoi(m.find("-dim")->second);
        if (m.find("-solver") != m.end())
            if (fieldSolvers.find(m.find("-solver")->second) != fieldSolvers.end())
                params.fieldSolver = fieldSolvers.find(m.find("-solver")->second)->second;
        if (m.find("-nbd") != m.end()) params.nIterBetweenDumps = std::stoi(m.find("-nbd")->second);
        if (m.find("-nCons") != m.end()) params.nConsSteps = std::stoi(m.find("-nCons")->second);

        if (task == Task::parallel) {
            if (m.find("-guard") != m.end()) params.guard = std::stoi(m.find("-guard")->second);
            if (m.find("-nPar") != m.end()) params.nParSteps = std::stoi(m.find("-nPar")->second);
            if (m.find("-mask") != m.end())
                if (maskMap.find(m.find("-mask")->second) != maskMap.end())
                    params.mask = maskMap.find(m.find("-mask")->second)->second;
            if (m.find("-filter") != m.end()) {
                if (m.find("-filter")->second == "on")
                    params.filter.state = Filter::State::on;
                else if (m.find("-filter")->second == "off")
                    params.filter.state = Filter::State::off;
            }

            int npx = mpiWrapper.MPISize().x, npy = mpiWrapper.MPISize().y, npz = mpiWrapper.MPISize().z;
            if (m.find("-npx") != m.end()) npx = std::stoi(m.find("-npx")->second);
            if (m.find("-npy") != m.end()) npy = std::stoi(m.find("-npy")->second);
            if (m.find("-npz") != m.end()) npz = std::stoi(m.find("-npz")->second);
            return mpiWrapper.CheckAndSetSize(npx, npy, npz);
        }
        return Status::OK;
    }
};