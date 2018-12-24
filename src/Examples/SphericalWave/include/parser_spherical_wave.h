#pragma once
#include <map>
#include "parser_for_my_test.h"
#include "spherical_wave.h"
#include "mask.h"
#include "filter.h"
#include "field_solver.h"

class ParserSphericalWave : public ParserForMyTest {
public:

    void help(ParametersForTest& p1, Task task) override {
        ParserForMyTest::help(p1, task);
        ParametersForSphericalWave& p = static_cast<ParametersForSphericalWave&>(p1);

        std::cout <<
            "-omega                      set frequency of source, default value is " << p.omega << "\n" <<
            "-omegaEnv                   set frequency of envelop, default value is " << p.omegaEnv << "\n" <<
            "-T                          set working time of source, default value is " << p.T << "\n" <<
            "-TCoord                     set width of source, default value is " << p.Tx << "\n" <<
            "-TCoordN                    set width of source (number of cells), default value is " << p.Tx / p.d << "\n" <<
            "-nIterS                     set working time of source using num of iterations, default value is " << p.T / p.dt << "\n" <<
            std::endl;
    }

    Status saveArgs(ParametersForTest& p, Task task) override {
        Status s = ParserForMyTest::saveArgs(p, task);
        if (s == Status::ERROR || s == Status::STOP) return s;

        ParametersForSphericalWave& params = static_cast<ParametersForSphericalWave&>(p);

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

        return Status::OK;

    }
};