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
            "-omenv                      set frequency of envelop, default value is " << p.omegaEnv << "\n" <<
            "-ts                         set working time of source, default value is " << p.T << "\n" <<
            "-tw                         set width of source, default value is " << p.Tx << "\n" <<
            std::endl;
    }

    Status saveArgs(ParametersForTest& p, Task task) override {
        Status s = ParserForMyTest::saveArgs(p, task);
        if (s == Status::ERROR || s == Status::STOP) return s;

        ParametersForSphericalWave& params = static_cast<ParametersForSphericalWave&>(p);

        if (m.find("-omega") != m.end()) params.omega = std::stod(m.find("-omega")->second);
        if (m.find("-omenv") != m.end()) params.omegaEnv = std::stod(m.find("-omenv")->second);
        if (m.find("-ts") != m.end()) params.T = std::stod(m.find("-ts")->second);
        if (m.find("-ws") != m.end()) {
            params.Tx = std::stod(m.find("-ws")->second);
            params.Ty = params.Tx;
        }

        return Status::OK;

    }
};