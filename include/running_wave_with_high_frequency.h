#pragma once
#include "running_wave.h"


class RunningWaveWithHighFrequency: public RunningWave {
public:
    int Tx = 8*d; //период 

    RunningWaveWithHighFrequency(): RunningWave() {
        dir = "../../../files/running_wave_with_high_frequency/";
        SetEB();
    }

    virtual double funcB(double x, double t) {
        return sin(2 * constants::pi / Tx*(-constants::c*t - x));
    }

    virtual double funcE(double x, double t) {
        return sin(2 * constants::pi / Tx*(constants::c*t + x));
    }
};


