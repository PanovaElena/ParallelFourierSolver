#pragma once
#include <cmath>
#include "my_complex.h"

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
    }

    Type x() const { return data[0]; }
    Type y() const { return data[1]; }
    Type z() const { return data[2]; }

    vec3(const vec3<Type> &c) {
        data[0] = c.data[0];
        data[1] = c.data[1];
        data[2] = c.data[2];
    }

    vec3 operator+(const vec3 &v)  const {
        return vec3(x() + v.x(), y() + v.y(), z() + v.z());
    }
    vec3& operator+=(const vec3 &v) {
        data[0] += v.x();
        data[1] += v.y();
        data[2] += v.z();
        return *this;
    }
    vec3& operator-=(const vec3 &v) {
        data[0] -= v.x();
        data[1] -= v.y();
        data[2] -= v.z();
        return *this;
    }
    vec3 operator* (double b) const {
        return vec3(x()*b, y()*b, z()*b);
    }
    vec3 operator* (const vec3 &v)  const
    {
        return vec3(y()*v.z() - z()*v.y(), z()*v.x() - x()*v.z(), x()*v.y() - y()*v.x());
    }
    
    vec3 operator/ (double a)  const// !!!!!!!!!!!!!!!!!!!
    {
        if (a != 0 )
        {
            return vec3(x() / a, y() / a, z() / a);
        }
        throw "DIV BY 0!";
    }
    vec3& operator/= (double a)// !!!!!!!!!!!!!!!!!!!
    {
        if (a != 0)
        {
            data[0] /= a;
            data[1] /= a;
            data[2] /= a;
            return *this;
        }
        throw "DIV BY 0!";
    }

    double getNorm()  const
    {
        return sqrt(ScalarProduct(*this, *this));
    }
    friend bool operator==(const vec3& a, const vec3& b)  
    {
        return ((a.x() == b.x()) && (a.y() == b.y()) && (a.z() == b.z()));
    }
    friend bool operator!=(const vec3& a, const vec3& b) {
        return !(a == b);
    }

    static double ScalarProduct(const vec3<double>& a, const vec3<double>& b) {
        return (a.data[0] * b.data[0] + a.data[1] * b.data[1] + a.data[2] * b.data[2]);
    }
    static double ScalarProduct(const vec3<MyComplex>& a, const vec3<MyComplex>& b) {
        return (double)(a.data[0] * b.data[0].Conjugate() + a.data[1] * b.data[1].Conjugate() + a.data[2] * b.data[2].Conjugate());
    }
    static vec3<double> VectorProduct(const vec3<double>& a, const vec3<double>& b) {
        double c1, c2, c3;
        c1 = a.data[1] * b.data[2] - a.data[2] * b.data[1];
        c2 = a.data[2] * b.data[0] - a.data[0] * b.data[2];
        c3 = a.data[0] * b.data[1] - a.data[1] * b.data[0];
        return vec3<double>(c1, c2, c3);
    }
    static vec3<MyComplex> VectorProduct(const vec3<MyComplex>& a, const vec3<MyComplex>& b) {
        MyComplex c1, c2, c3;
        c1 = a.data[1] * b.data[2].Conjugate() - a.data[2] * b.data[1].Conjugate();
        c2 = a.data[2] * b.data[0].Conjugate() - a.data[0] * b.data[2].Conjugate();
        c3 = a.data[0] * b.data[1].Conjugate() - a.data[1] * b.data[0].Conjugate();
        return vec3<MyComplex>(c1, c2, c3);
    }
    vec3<Type> Normalize() {
        return (*this)*1/getNorm();
    }
};
