#pragma once
#include "vector3d.h"

class Source {
public:
    vec3<double> coord;
    double time;   // время работы источника
    double omega;    // частота источника
    double omegaEnv;    // частота огибающей (по времени)
    vec3<double> width;    // ширина источника
    double startTime;    // время начала работы источника

    Source() : coord(0, 0, 0), time(0), omega(1), omegaEnv(1),
        width(1), startTime(0) {}

    Source(vec3<double> _coord, double _startTime, double _time, double _omega, double _omegaEnv,
        vec3<double> _width) {
        coord = _coord;
        time = _time;
        omega = _omega;
        omegaEnv = _omegaEnv;
        width = _width;
        startTime = _startTime;
    }
    double getEndTime() { return startTime + time; }

    double getJ(vec3<double> coordinate, double time);
};
