#include "particle_push.h"
#include "field_interpolation.h"
#include "constants.h"
#include "grid2d.h"
#include "particle.h"
#include <cmath>

void RelocateInside(double ax, double ay, double bx, double by, Particle &part)
{
	vec3 vec = part.getC();
	if (vec.x >= bx)
		vec.x -= (bx - ax);
	else if (vec.x < ax)
		vec.x += (bx - ax);

	if (vec.y >= by)
		vec.y -= (by - ay);
	else if (vec.y < ay)
		vec.y += (by - ay);
	part.setC(vec);
}
void SetCurrentSpeedAndPos(const Grid2d & gr, Particle & part, double dt)
{
	vec3 Pnew = GetCurrentMomentum(gr, part, dt);
	part.setP(Pnew);
	part.updateCoord(dt);
	RelocateInside(gr.gax(), gr.gay(), gr.gbx(), gr.gby(), part);
}
void degVal(double &a)
{
	const double e = 100000.0;
	const double delta = 10.0;
	if (a > 0 && a > e) a-= delta;
	else if (a < 0 && a < -e) a+= delta;
}
vec3 GetCurrentMomentum(const Grid2d & gr, Particle & part, double dt)
{
	vec3 U, U1, U2, Udt, T, S, Unew, Pnew;
	const vec3 B = calcB(gr, part), E = calcE(gr, part);
	vec3 V = part.getV();
	double v_len = V.getLength();
	if (v_len >= constants::c)
	{
		degVal(V.x);
		degVal(V.y);
		v_len = V.getLength();
	}
	const double m = part.getM(), mc = constants::c*m;
	const double q = part.getQ();

	//U- == U1	U+ == U2
	U = V / (sqrt(constants::c - v_len)*sqrt(constants::c + v_len)); // p = ( m*V )/( sqrt(1 - (len(V)/c)^2 ))
	const double g = sqrt(U.getSqLength()+ 1.0);
	U1 = U + E*(q*dt*0.5 / mc);
	T = (B * q * 0.5*dt) / (g * mc);//*dt!!!!!!!!!!!!!!!!!!!!!!!
	Udt = U1 + U1 * T;
	S = (T * 2) / (T.getSqLength() + 1);
	U2 = U1 + Udt*S;// - -> +
	Unew = U2 + E*(q*dt*0.5 / mc);
	Pnew = Unew*mc;
	return Pnew;
}
