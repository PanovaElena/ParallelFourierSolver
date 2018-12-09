#pragma once
#include <cmath>
#include "my_complex.h"
#include <iostream>
#include <string>


inline double absSquare(double a) {
    return a*a;
}
inline double absSquare(MyComplex a) {
    return a.GetReal()*a.GetReal()+ a.GetImag()*a.GetImag();
}

template <class Type=double>
struct vec3
{
    Type x;
    Type y;
    Type z;

    vec3() {};

    vec3(Type a, Type b, Type c) : x(a), y(b), z(c) {
    };

    explicit vec3(Type a) : x(a), y(a), z(a) {
    };

    vec3& operator=(const vec3 & v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    Type& operator[](int i) {
        switch (i) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: return x;
        }
    };

    Type get_x() const { return x; }
    Type get_y() const { return y; }
    Type get_z() const { return z; }

    vec3(const vec3<Type> &c) : x(c.x), y(c.y), z(c.z) {
    };

    template<class T>
    operator vec3<T> () {
        return vec3<T>(x, y, z);
    }

    vec3 operator+(const vec3 &v)  const {
        return vec3(x + v.get_x(), y + v.get_y(), z + v.get_z());
    };
    vec3& operator+=(const vec3 &v) {
        x += v.get_x();
        y += v.get_y();
        z += v.get_z();
        return *this;
    };
    vec3 operator-(const vec3 &v)  const {
        return vec3(x - v.get_x(), y - v.get_y(), z - v.get_z());
    };
    vec3& operator-=(const vec3 &v) {
        x -= v.get_x();
        y -= v.get_y();
        z -= v.get_z();
        return *this;
    };

    vec3 operator* (Type b) const {
        return vec3(x*b, y*b, z*b);
    };
    friend vec3 operator* (Type b, vec3 v) {
        return v*b;
    };
    //by components
    vec3 operator*(const vec3 &v)  const {
        return vec3(x * v.get_x(), y * v.get_y(), z * v.get_z());
    };
    vec3& operator*=(const Type &v) {
        x *= v;
        y *= v;
        z *= v;
        return *this;
    };
    vec3 operator/ (Type b) const {
        return vec3(x/b, y/b, z/b);
    };
    //by components
    vec3 operator/(const vec3 &v)  const {
        return vec3(x / v.get_x(), y / v.get_y(), z / v.get_z());
    };
    //by components
    vec3 operator%(const vec3 &v)  const {
        return vec3(x % v.get_x(), y % v.get_y(), z % v.get_z());
    };

    friend bool operator==(const vec3& a, const vec3& b)  
    {
        return ((a.get_x() == b.get_x()) && (a.get_y() == b.get_y()) && (a.get_z() == b.get_z()));
    };

    friend bool operator!=(const vec3& a, const vec3& b) {
        return !(a == b);
    };

	double getNorm()  const
	{
		return sqrt(absSquare(x)+ absSquare(y)+ absSquare(z));
	};
    static Type ScalarProduct(const vec3& a, const vec3& b) {
        return (a.x * b.x + a.y * b.y + a.z * b.z);
    };
    static vec3 VectorProduct(const vec3& a, const vec3& b) {
        Type c1, c2, c3;
        c1 = a.y * b.z - a.z * b.y;
        c2 = a.z * b.x - a.x * b.z;
        c3 = a.x * b.y - a.y * b.x;
        return vec3(c1, c2, c3);
    };
    vec3 Normalize() {
        if (getNorm() == 0) return *this;
        (*this)=(*this)*(1.0/getNorm());
        return *this;
    };

    friend std::ostream& operator<<(std::ostream& ost, vec3& vec) {
        ost << std::to_string(vec);
        return ost;
    }

    friend std::string to_string(vec3<Type>& vec) {
        std::string str="("+ std::to_string(vec.x) + "," + std::to_string(vec.y) + "," +
            std::to_string(vec.z) + ")";
        return str;
    }
};