#pragma once
#include "vec.h"
#include "grid3d.h"
#include "simple_types_and_constants.h"

typedef double (vec3<double>::*MethodCoord)() const;
typedef vec3<double> Node::* MemberOfNode;

inline MemberOfNode GetField(Field f) {
    switch (f) {
    case E:
        return &Node::E;
    case B:
        return &Node::B;
    case J:
        return &Node::J;
    default:
        return &Node::E;
    }
}

inline MethodCoord GetCoord(Coordinate coord) {
    switch (coord) {
    case x:
        return &vec3<double>::x;
    case y:
        return &vec3<double>::y;
    case z:
        return &vec3<double>::z;
    default:
        return &vec3<double>::x;
    }
}
