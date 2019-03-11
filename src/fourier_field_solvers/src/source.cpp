#include <cmath>
#include "source.h"
#include "physical_constants.h"

double Source::GetJ(vec3<double> coordinate, double t) {
    if (t > time + startTime || t < startTime) return 0;
    if (coordinate.x > coord.x + 0.5*width.x ||
        coordinate.x < coord.x - 0.5*width.x ||
        coordinate.y > coord.y + 0.5*width.y ||
        coordinate.y < coord.y - 0.5*width.y ||
        coordinate.z > coord.z + 0.5*width.z ||
        coordinate.z < coord.z - 0.5*width.z)
        return 0;
    return pow(cos(constants::pi*(coordinate.x - coord.x) / width.x), 2) *
        pow(cos(constants::pi*(coordinate.y - coord.y) / width.y), 2) *
        pow(cos(constants::pi*(coordinate.z - coord.z) / width.z), 2) *
        sin(omega*(t - startTime))*
        pow(sin(omegaEnv*(t - startTime)), 2);
}
