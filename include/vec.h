#pragma once
#include <cmath>

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

    double getSqLength()  const
    {
        return x()*x() + y()*y() + z()*z();
    }
    double getLength()  const
    {
        return sqrt(x()*x() + y()*y() + z()*z());
    }
    friend bool operator==(const vec3& a, const vec3& b)  
    {
        return ((a.x() == b.x()) && (a.y() == b.y()) && (a.z() == b.z()));
    }
    friend bool operator!=(const vec3& a, const vec3& b) {
        return !(a == b);
    }
};

struct vec2i
{
    int x;
    int y;
    vec2i() :x(0), y(0) {};
    vec2i(int a, int b):x(a), y(b){};
    vec2i(const vec2i &c) : x(c.x), y(c.y) {}
};