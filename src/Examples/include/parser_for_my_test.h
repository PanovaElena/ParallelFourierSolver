#pragma once
#include "command_line_parser.h"
#include "parameters_for_test.h"
#include "vec.h"

class ParserForMyTest: public CommandLineParser {
public:
    void help(ParametersForTest& p1, Task task) override {
        ParametersForMyTest& p = static_cast<ParametersForMyTest&>(p1);

        std::cout <<
            "-help                    get help\n" <<
            "-dt                      set time step, default value is " << p.dt << "\n" <<
            "-nx, -ny, -nz, -n        set size of grid, default value is "<< p.n << "\n" <<
            "-d                       set grid spacing, default value is " << p.d << "\n" <<
            "-solver                  set solver (\"PSTD\", \"PSATD\", \"FDTD\" or \"PSATD_omp\"), default value is \"PSATD\"\n" <<
            //"-nbd                     set number of iterations between dumps, default value is " << p.nIterBetweenDumps << "\n" <<
            "-nCons                   set number of consistent steps, default value is " << p.nConsSteps << "\n";
        if (task == Task::parallel) {
            std::cout <<
                "-nPar                       set number of parallel steps, default value is " << p.nParSteps << "\n" <<
                "-gx, -gy, -gz, -g           set width of guard, default value is " << p.guard << "\n" <<
                "-mask                       set mask (\"simple\" or \"smooth\"), default value is "<< p.mask.to_string() <<"\n" <<
                "-mwx, -mwy, -mwz, -mw       set width of mask (for \"smooth\"), default value is " << p.mask.getMaskWidth() << "\n" <<
                "-filter                     set on or off for low-frequency filter (\"on\" or \"off\"), default value is " << p.filter.to_string() << "\n" <<
                "-fwx, -fwy, -fwz, -fw       set width of low-frequency filter, default value is " << p.filter.getWidth() << "\n" <<
                "-fnzx, -fnzy, -fnzz, -fnz   set num of frequences which should be set to zeros for low-frequency filter, default value is " << p.filter.getNumZeroFreq() << "\n" <<
                "-npx, -npy, -npz, -np       set num of domains for every dimension, default value is (MPISize,1,1)" << "\n";
        }
    }

    Status saveArgs(ParametersForTest& p, Task task) override {

        ParametersForMyTest& params = static_cast<ParametersForMyTest&>(p);

        if (m.find("-d") != m.end()) params.d = std::stod(m.find("-d")->second);

        if (m.find("-nx") != m.end()) params.n.x = std::stoi(m.find("-nx")->second);
        if (m.find("-ny") != m.end()) params.n.y = std::stoi(m.find("-ny")->second);
        if (m.find("-nz") != m.end()) params.n.z = std::stoi(m.find("-nz")->second);
        if (m.find("-n") != m.end()) params.n = vec3<int>(std::stoi(m.find("-n")->second));

        if (m.find("-dt") != m.end()) params.dt = std::stod(m.find("-dt")->second);
        if (m.find("-solver") != m.end())
            if (FieldSolverMap.find(m.find("-solver")->second) != FieldSolverMap.end())
                params.fieldSolver = FieldSolverMap.find(m.find("-solver")->second)->second;
        //if (m.find("-nbd") != m.end()) params.nIterBetweenDumps = std::stoi(m.find("-nbd")->second);
        if (m.find("-nCons") != m.end()) params.nConsSteps = std::stoi(m.find("-nCons")->second);

        if (task == Task::parallel) {
            int npx = mpiWrapper.MPISize().x, npy = mpiWrapper.MPISize().y, npz = mpiWrapper.MPISize().z;
            if (m.find("-npx") != m.end()) npx = std::stoi(m.find("-npx")->second);
            if (m.find("-npy") != m.end()) npy = std::stoi(m.find("-npy")->second);
            if (m.find("-npz") != m.end()) npz = std::stoi(m.find("-npz")->second);
            if (m.find("-np") != m.end()) npx = npy = npz = std::stoi(m.find("-np")->second);

            if (m.find("-gx") != m.end()) params.guard.x = std::stoi(m.find("-gx")->second);
            if (m.find("-gy") != m.end()) params.guard.y = std::stoi(m.find("-gy")->second);
            if (m.find("-gz") != m.end()) params.guard.z = std::stoi(m.find("-gz")->second);
            if (m.find("-g") != m.end()) params.guard = vec3<int>::getVecIfLess(vec3<int>(std::stoi(m.find("-g")->second)), params.n);
            else params.guard = vec3<int>::getVecIfLess(params.guard, params.n);

            if (m.find("-nPar") != m.end()) params.nParSteps = std::stoi(m.find("-nPar")->second);

            if (m.find("-mask") != m.end())
                if (MaskMap.find(m.find("-mask")->second) != MaskMap.end())
                    params.mask = MaskMap.find(m.find("-mask")->second)->second;
            if (m.find("-mwx") != m.end()) params.mask.getMaskWidth().x = std::stoi(m.find("-mwx")->second);
            if (m.find("-mwy") != m.end()) params.mask.getMaskWidth().y = std::stoi(m.find("-mwy")->second);
            if (m.find("-mwz") != m.end()) params.mask.getMaskWidth().z = std::stoi(m.find("-mwz")->second);
            if (m.find("-mw") != m.end()) params.mask.setMaskWidth(vec3<int>::getVecIfLess(vec3<int>(std::stoi(m.find("-mw")->second)), params.n));
            else params.mask.setMaskWidth(vec3<int>::getVecIfLess(params.mask.getMaskWidth(), params.n));

            if (m.find("-filter") != m.end()) {
                if (m.find("-filter")->second == "on")
                    params.filter.state = Filter::State::on;
                else if (m.find("-filter")->second == "off")
                    params.filter.state = Filter::State::off;
            }
            if (m.find("-fwx") != m.end()) params.filter.getWidth().x = std::stoi(m.find("-fwx")->second);
            if (m.find("-fwy") != m.end()) params.filter.getWidth().y = std::stoi(m.find("-fwy")->second);
            if (m.find("-fwz") != m.end()) params.filter.getWidth().z = std::stoi(m.find("-fwz")->second);
            if (m.find("-fw") != m.end()) params.filter.setWidth(vec3<int>::getVecIfLess(vec3<int>(std::stoi(m.find("-fw")->second)), params.n));
            else params.filter.setWidth(vec3<int>::getVecIfLess(params.filter.getWidth(), params.n));

            if (m.find("-fnzx") != m.end()) params.filter.getNumZeroFreq().x = std::stoi(m.find("-fnzx")->second);
            if (m.find("-fnzy") != m.end()) params.filter.getNumZeroFreq().y = std::stoi(m.find("-fnzy")->second);
            if (m.find("-fnzz") != m.end()) params.filter.getNumZeroFreq().z = std::stoi(m.find("-fnzz")->second);
            if (m.find("-fnz") != m.end()) params.filter.setNumZeroFreq(vec3<int>::getVecIfLess(vec3<int>(std::stoi(m.find("-fnz")->second)), params.n));
            else params.filter.setNumZeroFreq(vec3<int>::getVecIfLess(params.filter.getNumZeroFreq(), params.n));

            return mpiWrapper.CheckAndSetSize(npx, npy, npz);
        }
        return Status::OK;
    }
};
