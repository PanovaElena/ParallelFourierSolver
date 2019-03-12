#pragma once
#include <cmath>
#include <string>
#include "physical_constants.h"
#include "grid3d.h"

typedef void(*FilterFunc) (Grid3d& gr, vec3<int> maskWidth, vec3<int> numZeroFreq);

void lowFrequencyFilter(Grid3d& gr, vec3<int> maskWidth, vec3<int> numZeroFreq);

class Filter {
public:
    enum State {
        on,
        off
    };

private:
    vec3<int> maskWidth;
    vec3<int> numZeroFreq;

public:
    Filter(vec3<int> _maskWidth, vec3<int> _numZeroFreq) :maskWidth(_maskWidth), numZeroFreq(_numZeroFreq) {}
    Filter() {}

    void setParams(vec3<int> _maskWidth, vec3<int> _numZeroFreq) {
        maskWidth = _maskWidth;
        numZeroFreq = _numZeroFreq;
    }
    void setWidth(vec3<int> w) {
        maskWidth = w;
    }
    void setNumZeroFreq(vec3<int> nzf) {
        numZeroFreq = nzf;
    }
    vec3<int>& getWidth() {
        return maskWidth;
    }
    vec3<int>& getNumZeroFreq() {
        return numZeroFreq;
    }

    State state = off;
    void operator() (Grid3d& gr) {
        if (state == on) lowFrequencyFilter(gr, maskWidth, numZeroFreq);
    }
    std::string to_string() {
        switch (state) {
        case on:
            return "on";
        case off:
            return "off";
        default:
            return "off";
        }
    }
};