#pragma once
#include "vector3d.h"

class Source {
    vec3<double> coord;
    double time;   // время работы источника
    double omega;    // частота источника
    double omegaEnv;    // частота огибающей (по времени)
    vec3<double> width;    // ширина источника
    double startTime;    // время начала работы источника

public:
    Source() : coord(0, 0, 0), time(0), omega(1), omegaEnv(1),
        width(1), startTime(0) {}

    Source(vec3<double> _coord, double _T, double _omega, double _omegaEnv,
        vec3<double> _width, double _startTime, double _endTime) {
        coord = _coord;
        time = _T;
        omega = _omega;
        omegaEnv = _omegaEnv;
        width = _width;
        startTime = _startTime;
    }
    double& getStartTime() { return startTime; }
    double getEndTime() { return startTime + time; }
    double& getTime() { return time; }
    vec3<>& getCoord() { return coord; }
    vec3<>& getWidth() { return width; }
    double& getFreq() { return omega; }
    double& getFreqEnvelope() { return omegaEnv; }

    double getJ(vec3<double> coordinate, double time);
};
