#pragma once
#include "vec.h"
class Particle
{
private:
	vec3 c; //coord
	vec3 v;//speed
	double q;//заряд частицы
	double m;//масса частицы
	double factor = 1.0;

public:
	Particle();
	Particle(vec3 c1, vec3 v1, double q, double m);
	Particle(vec3 c1, vec3 v1, double _q, double _m, double _factor);

	double getM() const;
	double getQ() const;
	vec3 getC() const;
	vec3 getV() const;
	vec3 getP() const;


	void setM(double m);
	void setQ(double q);
	void setC(const vec3& c);
	void setV(const vec3& v);
	void setP(const vec3& p);
	void setFactor(double f);
	void updateCoord(double t);

};