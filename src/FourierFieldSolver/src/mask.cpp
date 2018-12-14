#include "mask.h"

double simpleMask(vec3<int> ind, vec3<int> mainSize, vec3<int> guardSize, int maskWidth) {
    if (ind.x < guardSize.x || ind.x >= mainSize.x + guardSize.x ||
        ind.y < guardSize.y || ind.y >= mainSize.y + guardSize.y ||
        ind.z < guardSize.z || ind.z >= mainSize.z + guardSize.z) return 0;
    return 1;
}