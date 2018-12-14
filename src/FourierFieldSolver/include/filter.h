#pragma once
#include "physical_constants.h"
#include "cmath"
#include "grid3d.h"
#include <string>

typedef void(*FilterFunc) (Grid3d& gr, int maskWidth, int numZeroFreq);

void LowFrequencyFilter(Grid3d& gr, int maskWidth, int numZeroFreq);

class Filter {
public:
    enum State {
        on,
        off
    };
    State state = off;
    void operator() (Grid3d& gr, int maskWidth, int numZeroFreq) {
        if (state == on) LowFrequencyFilter(gr, maskWidth, numZeroFreq);
    }
    std::string to_string() {
        switch (state) {
        case on:
            return "on";
        case off:
            return "off";
        }
    }
};