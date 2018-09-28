#pragma once
#include "physical_constants.h"
#include "cmath"

typedef double(*Mask) (int i, int mainDomainSize, int guardSize, int maskWidth);

inline double simpleMask(int i, int mainDomainSize, int guardSize, int maskWidth) {
    if (i < guardSize || i>mainDomainSize + guardSize) return 0;
    return 1;
}

inline double maskSineSquare(int i, int mainDomainSize, int guardSize, int maskWidth) {
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
