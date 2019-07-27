#pragma once
#include <map>
#include "vector3d.h"

typedef double(*MaskFunc) (vec3<int> ind, vec3<int> mainSize, vec3<int> guardSize, vec3<int> maskWidth);

double simpleMask(vec3<int> ind, vec3<int> mainSize, vec3<int> guardSize, vec3<int> maskWidth);
double smoothMask(vec3<int> ind, vec3<int> mainSize, vec3<int> guardSize, vec3<int> maskWidth);

class Mask {
public:
    MaskFunc func;
    std::string str;
    vec3<int> maskWidth;

    Mask() {}
    Mask(MaskFunc f, std::string s) : func(f), str(s) {
    }
    Mask(MaskFunc f, std::string s, vec3<int> _maskWidth) : func(f), str(s), maskWidth(_maskWidth) {
    }
    vec3<int> getMaskWidth() const {
        return maskWidth;
    }
    void setMaskWidth(vec3<int> mw) {
        maskWidth = mw;
    }
    std::string to_string() const {
        return str;
    }
    double operator() (vec3<int> ind, vec3<int> mainSize, vec3<int> guardSize) {
        return func(ind, mainSize, guardSize, maskWidth);
    }
    friend double operator==(const Mask& m1, const Mask& m2) {
        return (m1.func == m2.func);
    }
    friend double operator!=(const Mask& m1, const Mask& m2) {
        return (m1 != m2);
    }
};

const Mask SimpleMask(simpleMask, "simple");
const Mask SmoothMask(smoothMask, "smooth");

const std::map<std::string, Mask> MaskMap =
{ { "simple", SimpleMask },{ "smooth", SmoothMask } };