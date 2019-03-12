#pragma once
#include <map>
#include "parser_for_my_test.h"
#include "running_wave.h"
#include "mask.h"
#include "filter.h"
#include "field_solver.h"

class ParserRunningWave : public ParserForMyTest {
public:
    void help(ParametersForTest& p1, Task task) override {
        ParserForMyTest::help(p1, task);
        ParametersForRunningWave& p = static_cast<ParametersForRunningWave&>(p1);

        std::cout <<
            "-lambda                     set length of wave (more important, then \"-lambdaN\"), default value is " << p.lambda << "\n" <<
            "-angle                      set angle in degrees between axis OX and dimension of wave, default value is " << p.angle << "\n" <<
            "-dim                        set dimension of output data (1 or 2), default value is " << p.dimensionOfOutputData << "\n" <<
            std::endl;
    }

    Status saveArgs(ParametersForTest& p, Task task) override {
        Status s = ParserForMyTest::saveArgs(p, task);
        if (s == Status::ERROR || s == Status::STOP) return s;

        ParametersForRunningWave& params = static_cast<ParametersForRunningWave&>(p);

        if (m.find("-lambda") != m.end()) params.lambda = std::stod(m.find("-lambda")->second);
        if (m.find("-angle") != m.end()) params.angle = constants::pi*std::stod(m.find("-angle")->second) / 180;
        if (m.find("-dim") != m.end()) params.dimensionOfOutputData = std::stoi(m.find("-dim")->second);

        return Status::OK;
    }

};