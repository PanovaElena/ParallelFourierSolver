#pragma once
#include "physical_constants.h"
#include "cmath"
#include "vec.h"

typedef double(*Mask) (vec3<int> ind, vec3<int> mainDomainSize, vec3<int> guardSize, int maskWidth);

inline double simpleMask(vec3<int> ind, vec3<int> mainDomainSize, vec3<int> guardSize, int maskWidth) {
    if (ind.x < guardSize.x || ind.x>mainDomainSize.x + guardSize.x || 
        ind.y < guardSize.y || ind.y>mainDomainSize.y + guardSize.y ||
        ind.z < guardSize.z || ind.z>mainDomainSize.z + guardSize.z) return 0;
    return 1;
}
//заглушка
inline double maskSineSquare(vec3<int> ind, vec3<int> mainDomainSize, vec3<int> guardSize, int maskWidth) {
    /*if (i < guardSize - maskWidth / 2 || i > guardSize + mainDomainSize + maskWidth / 2 + 1)
        return 0;
    if (i <= guardSize + maskWidth / 2 && i >= guardSize - maskWidth / 2) {
        i -= guardSize - maskWidth / 2;
        return sin(i*constants::pi / (2 * maskWidth))*sin(i*constants::pi / (2 * maskWidth));
    }
    if (i <= guardSize + mainDomainSize + maskWidth / 2 + 1 && i >= guardSize + mainDomainSize - maskWidth / 2 + 1) {
        i -= guardSize + mainDomainSize - maskWidth / 2 + 1;
        return cos(i*constants::pi / (2 * maskWidth))*cos(i*constants::pi / (2 * maskWidth));
    }
    return 1;*/
    return simpleMask(ind, mainDomainSize, guardSize, maskWidth);
}
