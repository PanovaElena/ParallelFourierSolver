#pragma once
#include "vec.h"
#include "grid3d.h"

typedef double (vec3<double>::*MethodCoord)() const;
typedef vec3<double> node::* MemberOfNode;

inline MemberOfNode GetField(int f) {
	switch (f) {
	case 0:
		return &node::E;
	case 1:
		return &node::B;
	}
}

inline MethodCoord GetCoord(int coord) {
	switch (coord) {
	case 0:
		return &vec3<double>::x;
	case 1:
		return &vec3<double>::y;
	case 2:
		return &vec3<double>::z;
	default:
		return &vec3<double>::x;
	}
}
