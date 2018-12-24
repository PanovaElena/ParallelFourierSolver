#pragma once
#include "physical_constants.h"
#include "cmath"
#include "vec.h"
#include <map>

typedef double(*MaskFunc) (vec3<int> ind, vec3<int> mainSize, vec3<int> guardSize, vec3<int> maskWidth);

double simpleMask(vec3<int> ind, vec3<int> mainSize, vec3<int> guardSize, vec3<int> maskWidth);
double smoothMask(vec3<int> ind, vec3<int> mainSize, vec3<int> guardSize, vec3<int> maskWidth);

class Mask {
    MaskFunc func;
    std::string str;
    vec3<int> maskWidth;
public:

    Mask() {}
    Mask(MaskFunc f, std::string s) : func(f), str(s) {
    }
    Mask(MaskFunc f, std::string s, vec3<int> _maskWidth): func(f), str(s), maskWidth(_maskWidth) {
    }
    vec3<int>& getMaskWidth() {
        return maskWidth;
    }
    void setMaskWidth(vec3<int> mw) {
        maskWidth = mw;
    }
    std::string to_string() {
        return str;
    }
    double operator() (vec3<int> ind, vec3<int> mainSize, vec3<int> guardSize) {
        return func(ind, mainSize, guardSize, maskWidth);
    }
};

const Mask SimpleMask(simpleMask, "simple");
const Mask SmoothMask(smoothMask, "smooth");

const std::map<std::string, Mask> MaskMap =
{ { "simple", SimpleMask },{ "smooth", SmoothMask } };