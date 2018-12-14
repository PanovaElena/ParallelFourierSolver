#pragma once
#include "physical_constants.h"
#include "cmath"
#include "vec.h"
#include <map>

typedef double(*MaskFunc) (vec3<int> ind, vec3<int> mainSize, vec3<int> guardSize, int maskWidth);

double simpleMask(vec3<int> ind, vec3<int> mainSize, vec3<int> guardSize, int maskWidth);
//double SmoothMask(int i, int mainSize, int guardSize, int maskWidth);

class Mask {
    MaskFunc func;
    std::string str;
public:
    Mask() {}
    Mask(MaskFunc f, std::string s) {
        func = f;
        str = s;
    }
    std::string to_string() {
        return str;
    }
    double operator() (vec3<int> ind, vec3<int> mainSize, vec3<int> guardSize, int maskWidth) {
        return func(ind, mainSize, guardSize, maskWidth);
    }
};

const Mask MaskSimple(simpleMask, "simple");
//const Mask MaskSmooth(SmoothMask, "smooth");

const std::map<std::string, Mask> maskMap =
{ { "simple", MaskSimple }/*,{ "smooth", MaskSmooth }*/ };