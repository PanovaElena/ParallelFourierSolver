#pragma once
#include <cmath>
typedef double fftw_complex[2];

template <class T>
struct vec3
{
	T x;
	T y;
	T z;

	vec3():x(0), y(0), z(0){}
	vec3(T a, T b, T c) :x(a),y(b),z(c) {};
	vec3(const vec3 &c) : x(c.x), y(c.y), z(c.z){}

	vec3 operator+(const vec3 &v)  const {
		return vec3(x + v.x, y + v.y, z + v.z);
	}
	vec3& operator+=(const vec3 &v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	vec3& operator-=(const vec3 &v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	vec3 operator* (double b) const {
		return vec3(x*b, y*b, z*b);
	}
	vec3 operator* (const vec3 &v)  const
	{
		return vec3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}
	
	vec3 operator/ (double a)  const// !!!!!!!!!!!!!!!!!!!
	{
		if (a != 0 )
		{
			return vec3(x / a, y / a, z / a);
		}
		throw "DIV BY 0!";
	}
	vec3& operator/= (double a)// !!!!!!!!!!!!!!!!!!!
	{
		if (a != 0)
		{
			x /= a;
			y /= a;
			z /= a;
			return *this;
		}
		throw "DIV BY 0!";
	}
	T operator[](int i) {
		switch (i) {
		case 0:return x;
		case 1:return y;
		case 2:return z;
		default:return 0;
		}
	}

	double getSqLength()  const
	{
		return x*x + y*y + z*z;
	}
	double getLength()  const
	{
		return sqrt(x*x + y*y + z*z);
	}
	friend bool operator==(const vec3& a, const vec3& b)  
	{
		return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z));
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