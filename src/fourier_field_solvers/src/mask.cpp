#include <cmath>
#include "physical_constants.h"
#include "mask.h"

double simpleMask(vec3<int> ind, vec3<int> mainSize, vec3<int> guardWidth, vec3<int> maskWidth) {
    if (ind.x < guardWidth.x || ind.x >= mainSize.x + guardWidth.x ||
        ind.y < guardWidth.y || ind.y >= mainSize.y + guardWidth.y ||
        ind.z < guardWidth.z || ind.z >= mainSize.z + guardWidth.z) return 0;
    return 1;
}

double f(int i, int mainSize, int guardSize, int maskWidth) {
    if (i < guardSize - maskWidth / 2 || i > guardSize + mainSize + maskWidth / 2)
        return 0;
    if (i <= guardSize + maskWidth / 2 && i >= guardSize - maskWidth / 2) {
        i -= guardSize - maskWidth / 2;
        return sin(i*constants::pi / (2 * maskWidth))*sin(i*constants::pi / (2 * maskWidth));
    }
    if (i <= guardSize + mainSize + maskWidth / 2 && i >= guardSize + mainSize - maskWidth / 2) {
        i -= guardSize + mainSize - maskWidth / 2;
        return cos(i*constants::pi / (2 * maskWidth))*cos(i*constants::pi / (2 * maskWidth));
    }
    return 1;
}

double smoothMask(vec3<int> ind, vec3<int> mainSize, vec3<int> guardSize, vec3<int> maskWidth) {
    double res = 1;
    if (maskWidth.x > 0) res *= f(ind.x, mainSize.x, guardSize.x, maskWidth.x);
    if (maskWidth.y > 0) res *= f(ind.y, mainSize.y, guardSize.y, maskWidth.y);
    if (maskWidth.z > 0) res *= f(ind.z, mainSize.z, guardSize.z, maskWidth.z);
    return res;
}