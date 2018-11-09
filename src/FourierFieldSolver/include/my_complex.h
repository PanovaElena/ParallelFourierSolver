#pragma once
#include "cmath"

class MyComplex {
    double data[2];
public:
    MyComplex() {
        data[0] = 0;
        data[1] = 0;
    }
    MyComplex(double real, double imag) {
        data[0] = real;
        data[1] = imag;
    }
    MyComplex(double a) {
        data[0] = a;
        data[1] = 0;
    }
    static MyComplex GetTrig(double r, double fi) {
        MyComplex z;
        z.data[0] = r*cos(fi);
        z.data[1] = r*sin(fi);
        return z;
    }
    void SetReal(double real) {
        data[0] = real;
    }
    void SetImag(double imag) {
        data[1] = imag;
    }
    double GetReal() {
        return data[0];
    }
    double GetImag() {
        return data[1];
    }
    friend MyComplex operator*(const MyComplex& z, const double a) {
        return MyComplex(z.data[0] * a, z.data[1] * a);
    }
    friend MyComplex operator*(const double a, const MyComplex& z) {
        return MyComplex(z.data[0] * a, z.data[1] * a);
    }
    friend MyComplex operator*(const MyComplex& z1, const MyComplex& z2) {
        return MyComplex(z1.data[0] * z2.data[0]-z1.data[1]*z2.data[1], z1.data[0] * z2.data[1] + z2.data[0] * z1.data[1]);
    }
    friend MyComplex operator/(const MyComplex& z, const double a) {
        return MyComplex(z.data[0] / a, z.data[1] / a);
    }
    friend MyComplex operator+(const MyComplex& z1, const MyComplex& z2) {
        return MyComplex(z1.data[0] + z2.data[0], z1.data[1] + z2.data[1]);
    }
    MyComplex& operator+=(const MyComplex& z2) {
        data[0] += z2.data[0];
        data[1] += z2.data[1];
        return *this;
    }
    MyComplex& operator*=(const MyComplex& z2) {
        (*this) = (*this)*z2;
        return *this;
    }
    MyComplex& operator-=(const MyComplex& z2) {
        data[0] -= z2.data[0];
        data[1] -= z2.data[1];
        return *this;
    }
    friend MyComplex operator-(const MyComplex& z1, const MyComplex& z2) {
        return z1 + (-1)*z2;
    }
    friend bool operator==(const MyComplex& z1, const MyComplex& z2) {
        return (z1.data[0] == z2.data[0] && z1.data[1] == z2.data[1]);
    }
    friend bool operator!=(const MyComplex& z1, const MyComplex& z2) {
        return !(z1==z2);
    }
    MyComplex Conjugate() const{
        return MyComplex(data[0], -data[1]);
    }
    operator double() { return data[0]; }

};

inline double abs(MyComplex a) {
	return sqrt(a.GetReal()*a.GetReal() + a.GetImag()*a.GetImag());
}

const MyComplex complex_i(0, 1);
const MyComplex complex_0(0, 0);