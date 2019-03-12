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
            "-scx, -scy, -scz            set coordinate of source, default value is " << p.source.getCoord() << "\n" <<
            "-somega                     set frequency of source, default value is " << p.source.getFreq() << "\n" <<
            "-somenv                     set frequency of envelop for source, default value is " << p.source.getFreqEnvelope() << "\n" <<
            "-stime                      set working time of source, default value is " << p.source.getTime() << "\n" <<
            "-stimest                    set start time of source, default value is " << p.source.getStartTime() << "\n" <<
            "-swx, -swy, -swz            set width of source, default value is " << p.source.getWidth() << "\n" <<
            std::endl;
    }

    Status saveArgs(ParametersForTest& p, Task task) override {
        Status s = ParserForMyTest::saveArgs(p, task);
        if (s == Status::ERROR || s == Status::STOP) return s;

        ParametersForSphericalWave& params = static_cast<ParametersForSphericalWave&>(p);

        if (m.find("-scx") != m.end()) params.source.getCoord().x = std::stod(m.find("-scx")->second);
        if (m.find("-scy") != m.end()) params.source.getCoord().y = std::stod(m.find("-scy")->second);
        if (m.find("-scz") != m.end()) params.source.getCoord().z = std::stod(m.find("-scz")->second);

        if (m.find("-swx") != m.end()) params.source.getWidth().x = std::stod(m.find("-swx")->second);
        if (m.find("-swy") != m.end()) params.source.getWidth().y = std::stod(m.find("-swy")->second);
        if (m.find("-swz") != m.end()) params.source.getWidth().z = std::stod(m.find("-swz")->second);

        if (m.find("-somega") != m.end()) params.source.getFreq() = std::stod(m.find("-somega")->second);
        if (m.find("-somenv") != m.end()) params.source.getFreqEnvelope() = std::stod(m.find("-somenv")->second);
        if (m.find("-stime") != m.end()) params.source.getTime() = std::stod(m.find("-stime")->second);
        if (m.find("-stimest") != m.end()) params.source.getStartTime() = std::stod(m.find("-stimest")->second);

        return Status::OK;

    }
};