#pragma once
#include <cmath>
#include "my_complex.h"


inline double absSquare(double a) {
    return a*a;
}
inline double absSquare(MyComplex a) {
    return a.GetReal()*a.GetReal()+ a.GetImag()*a.GetImag();
}

template <class Type>
struct vec3
{
    Type data[3];

    vec3() {};

    vec3(Type a, Type b, Type c) {
        data[0] = a;
        data[1] = b;
        data[2] = c;
    };

    Type& operator[](int i) {
        return data[i];
    };

    Type x() const { return data[0]; }
    Type y() const { return data[1]; }
    Type z() const { return data[2]; }

    vec3(const vec3<Type> &c) {
        data[0] = c.data[0];
        data[1] = c.data[1];
        data[2] = c.data[2];
    };

    vec3 operator+(const vec3 &v)  const {
        return vec3(x() + v.x(), y() + v.y(), z() + v.z());
    };
    vec3 operator-(const vec3 &v)  const {
        return vec3(x() - v.x(), y() - v.y(), z() - v.z());
    };
    vec3& operator+=(const vec3 &v) {
        data[0] += v.x();
        data[1] += v.y();
        data[2] += v.z();
        return *this;
    };
    vec3& operator-=(const vec3 &v) {
        data[0] -= v.x();
        data[1] -= v.y();
        data[2] -= v.z();
        return *this;
    };

    friend vec3<MyComplex> operator* (MyComplex b, vec3<MyComplex> v);
    vec3<MyComplex> operator* (MyComplex b) const {
        return vec3<MyComplex>(x()*b, y()*b, z()*b);
    };
    vec3 operator* (double b) const {
        return vec3(x()*b, y()*b, z()*b);
    };
    friend vec3 operator* (double b, vec3 v) {
        return v*b;
    };


    friend bool operator==(const vec3& a, const vec3& b)  
    {
        return ((a.x() == b.x()) && (a.y() == b.y()) && (a.z() == b.z()));
    };
    friend bool operator!=(const vec3& a, const vec3& b) {
        return !(a == b);
    };

	double getNorm()  const
	{
		return sqrt(absSquare(x())+ absSquare(y())+ absSquare(z()));
	};
    static Type ScalarProduct(const vec3& a, const vec3& b) {
        return (a.data[0] * b.data[0] + a.data[1] * b.data[1] + a.data[2] * b.data[2]);
    };
    static vec3 VectorProduct(const vec3& a, const vec3& b) {
        Type c1, c2, c3;
        c1 = a.data[1] * b.data[2] - a.data[2] * b.data[1];
        c2 = a.data[2] * b.data[0] - a.data[0] * b.data[2];
        c3 = a.data[0] * b.data[1] - a.data[1] * b.data[0];
        return vec3(c1, c2, c3);
    };
    vec3 Normalize() {
        if (getNorm() == 0) return *this;
        (*this)=(*this)*(1.0/getNorm());
        return *this;
    };
};


inline vec3<MyComplex> operator* (MyComplex b, vec3<MyComplex> v) {
    return v*b;
};