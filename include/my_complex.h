#pragma once
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
	friend MyComplex operator*(const MyComplex z, const double a) {
		return MyComplex(z.data[0] * a, z.data[1] * a);
	}
	friend MyComplex operator*(const double a, const MyComplex z) {
		return MyComplex(z.data[0] * a, z.data[1] * a);
	}
	friend MyComplex operator*(const MyComplex z1, const MyComplex z2) {
		return MyComplex(z1.data[0] * z2.data[0]-z1.data[1]*z2.data[1], z1.data[0] * z2.data[1] + z2.data[0] * z1.data[1]);
	}
	friend MyComplex operator+(const MyComplex z1, const MyComplex z2) {
		return MyComplex(z1.data[0] + z2.data[0], z1.data[1] + z2.data[1]);
	}
	MyComplex& operator+=(const MyComplex z2) {
		data[0] += z2.data[0];
		data[1] += z2.data[1];
		return *this;
	}
	MyComplex& operator-=(const MyComplex z2) {
		data[0] -= z2.data[0];
		data[1] -= z2.data[1];
		return *this;
	}
	friend MyComplex operator-(const MyComplex z1, const MyComplex z2) {
		return z1 + (-1)*z2;
	}


};

const MyComplex complex_i(0, 1);