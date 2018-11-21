#pragma once
#include "vec.h"
#include "grid3d.h"
#include "simple_types_and_constants.h"

typedef vec3<double> Node::* MemberOfNode;

typedef double vec3<double>::* MemberOfVec;

template<class T>
inline vec3<T> Node::* GetField(Field f) {
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

template<>
inline vec3<MyComplex> Node::* GetField(Field f) {
    switch (f) {
    case E:
        return &Node::EF;
    case B:
        return &Node::BF;
    case J:
        return &Node::JF;
    default:
        return &Node::EF;
    }
}

template<class T>
inline T vec3<T>::* GetCoord(Coordinate coord) {
    switch (coord) {
    case x:
        return &vec3<T>::x;
    case y:
        return &vec3<T>::y;
    case z:
        return &vec3<T>::z;
    default:
        return &vec3<T>::x;
    }
}