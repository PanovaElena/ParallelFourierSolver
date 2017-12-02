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
	

};

const MyComplex compl_i(0, 1);