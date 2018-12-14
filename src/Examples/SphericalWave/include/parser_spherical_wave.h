#pragma once
#include <map>
#include "command_line_parser.h"
#include "spherical_wave.h"
#include "mask.h"
#include "filter.h"
#include "field_solver.h"

class ParserSphericalWave : public CommandLineParser {
public:

    void help(Task task) override {
        ParametersForSphericalWave p;
        std::cout <<
            "-help                    get help\n" <<
            "-dt                      set time step, default value is " << p.dt << "\n" <<
            "-omega                   set frequency of source, default value is " << p.omega << "\n" <<
            "-omegaEnv                set frequency of envelop, default value is " << p.omegaEnv << "\n" <<
            "-T                       set working time of source, default value is " << p.T << "\n" <<
            "-TCoord                  set width of source, default value is " << p.Tx << "\n" <<
            "-TCoordN                 set width of source (number of cells), default value is " << p.Tx / p.d << "\n" <<
            "-nIterS                  set working time of source using num of iterations, default value is " << p.T / p.dt << "\n" <<
            "-d                        set step of grid, default value is " << p.d << "\n" <<
            "-nx, -ny, -nz          set size of grid, default value is " << p.nx << ", " << p.ny << ", " << p.nz << "\n" <<
            "-solver                  set solver (\"PSTD\", \"PSATD\", \"FDTD\" or \"PSATD_omp\"), default value is \"PSATD\"\n" <<
            "-nbd                     set number of iterations between dumps, default value is " << p.nIterBetweenDumps << "\n" <<
            "-nCons                   set number of consistent steps, default value is " << p.nConsSteps << "\n";
        if (task == Task::parallel) {
            std::cout <<
                "-nPar                    set number of parallel steps, default value is " << p.nParSteps <<
                "-guard                   set width of guard, default value is " << p.guard << "\n" <<
                "-mask                    set mask (\"simple\" or \"smooth\"), default value is \"smooth\"\n" <<
                "-filter                  set on or off for low-frequency filter (\"on\" or \"off\"), default value is " << p.filter.to_string() << "\n" <<
                "-npx, -npy, -npz       set num of domains for every dimension, default value is sqrt3(MPISize)\n";
        }
        std::cout << std::endl;
    }

    Status saveArgs(ParametersForTest& p, Task task) override {
 
        ParametersForSphericalWave& params = static_cast<ParametersForSphericalWave&>(p);

        if (m.find("-d") != m.end()) params.d = std::stod(m.find("-d")->second);
        if (m.find("-nx") != m.end()) params.nx = std::stoi(m.find("-nx")->second);
        if (m.find("-ny") != m.end()) params.ny = std::stoi(m.find("-ny")->second);
        if (m.find("-nz") != m.end()) params.nz = std::stoi(m.find("-nz")->second);
        if (m.find("-dt") != m.end()) params.dt = std::stod(m.find("-dt")->second);
        if (m.find("-omega") != m.end()) params.omega = std::stod(m.find("-omega")->second);
        if (m.find("-omegaEnv") != m.end()) params.omegaEnv = std::stod(m.find("-omegaEnv")->second);
        if (m.find("-nIterS") != m.end()) params.T = std::stoi(m.find("-nIterS")->second)*params.dt;
        if (m.find("-T") != m.end()) params.T = std::stod(m.find("-T")->second);
        if (m.find("-TCoordN") != m.end()) {
            params.Tx = std::stoi(m.find("-TCoordN")->second)*params.d;
            params.Ty = params.Tx;
        }
        if (m.find("-TCoord") != m.end()) {
            params.Tx = std::stod(m.find("-TCoord")->second);
            params.Ty = params.Tx;
        }
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
            return mpiWrapper.CheckAndSetSize(npx, npy, npz);
        }
        return Status::OK;

    }
};