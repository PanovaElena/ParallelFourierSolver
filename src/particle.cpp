#include "particle.h"
#include "constants.h"
#include "vec.h"

Particle::Particle():c(), v(), q(0), m(0){}
Particle::Particle(vec3 c1, vec3 v1, double _q, double _m) :c(c1), v(v1), q(_q), m(_m) {}

Particle::Particle(vec3 c1, vec3 v1, double _q, double _m, double _factor): c(c1), v(v1), q(_q), m(_m), factor(_factor) {}

double Particle::getM() const
{
	return m*factor;
}
double Particle::getQ() const
{
	return q*factor;
}
vec3 Particle::getC() const
{
	return c;
}
vec3 Particle::getV() const
{
	return v;
}
vec3 Particle::getP() const
{

	return (v * getM()) / sqrt(1 - (v.getSqLength() / constants::speedLightSQ));
}
void Particle::setM(double m)
{
	this->m = m;
}
void Particle::setQ(double q)
{
	this->q = q;
}
void Particle::setC(const vec3& c)
{
	this->c = c;
}
void Particle::setV(const vec3& v)
{
	this->v = v;
}
void Particle::setP(const vec3 & p)
{
	double pmc = p.getSqLength() / (getM()*getM()*constants::speedLightSQ);
	v = p / (sqrt(pmc + 1) * getM());
}
void Particle::setFactor(double f)
{
	factor = f;
}
void Particle::updateCoord(double t)
{
	c += v*t;
}



