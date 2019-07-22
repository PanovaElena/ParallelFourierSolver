#pragma once
#include "vector3d.h"

class Source {
public:
    vec3<double> coord;
    double time;   // ����� ������ ���������
    double omega;    // ������� ���������
    double omegaEnv;    // ������� ��������� (�� �������)
    vec3<double> width;    // ������ ���������
    double startTime;    // ����� ������ ������ ���������

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
