#pragma once
#include "physical_constants.h"
#include "cmath"

class Mask {
public:
    virtual double func(int i, int mainDomainSize, int guardSize) = 0;
};

class SimpleMask : public Mask {
public:
    SimpleMask() {}

    virtual double func(int i, int mainDomainSize, int guardSize) {
        if (i < guardSize || i>mainDomainSize + guardSize) return 0;
        return 1;
    }
};

class MaskSineSquare : public Mask {
    int maskWidth;

public:
    MaskSineSquare(int _maskWidth) : maskWidth(_maskWidth) {}

    virtual double func(int i, int mainDomainSize, int guardSize) {
        if (i < guardSize - maskWidth / 2 || i > guardSize + mainDomainSize + maskWidth / 2 + 1)
            return 0;
        if (i <= guardSize + maskWidth / 2 && i >= guardSize - maskWidth / 2) {
            i -= guardSize - maskWidth / 2;
            return sin(i*constants::pi / (2 * maskWidth))*sin(i*constants::pi / (2 * maskWidth));
        }
        if (i <= guardSize + mainDomainSize + maskWidth / 2 + 1 && i >= guardSize + mainDomainSize - maskWidth / 2 + 1) {
            i -= guardSize + mainDomainSize - maskWidth / 2 + 1;
            return cos(i*constants::pi / (2 * maskWidth))*cos(i*constants::pi / (2 * maskWidth));
        }
        return 1;
    }
};
