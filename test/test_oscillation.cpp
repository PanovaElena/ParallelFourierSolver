#include "gtest.h"
#include "constants.h"
#include "particle_push.h"
#include "vec.h"
#include "particle.h"
#include "grid2d.h"
#include <cmath>
#include <iostream>
#include <fstream>

int sign(double a) {
	if (a > 0) return 1;
	if (a < 0) return -1;
	return 0;
}

class TestOscillation: public testing::Test {
	char* str;
public:
	double er = 0.1;
	double exp_px, exp_py, exp_vx, exp_vy, exp_cx, exp_cy;
	Grid2d *gr;
	Particle *p;
	double B0, p0, l, k, m, q;
	double x, y, nx, ny;
	int n;
	double dt;

	TestOscillation():str("######### SIGNUM IS NOT RIGHT! "){
		gr = NULL;
		p = NULL;
	}

	void SetVar(double _B0, double _p0, double _q, double _m) {
		B0 = _B0; p0 = _p0; q = _q; m = _m;
		x = (double)((int)(p0*constants::c / abs(q) / B0) + 20); y = x; 
		nx = 2 * x; ny = 2 * y;
		l = constants::c / q / B0;
		k = 1 / (m*sqrt(1 + p0*p0 / m*m*constants::speedLightSQ));
		if (gr != NULL) delete gr;
		gr = new Grid2d((int)nx, (int)ny, -x, -y, x, y, vec3(0, 0, B0), vec3(0, 0, 0));
		if (p != NULL) delete p;
		p = new Particle(vec3(0, l*p0, 0), vec3(k*p0, 0, 0), q, m);
		n=50;
		dt = constants::pi*abs(l) / k / n;
	}

	void SignPIsRight(double exp_px, double exp_py) {
		if (sign(p->getP().x) != sign(exp_px) || sign(p->getP().y) != sign(exp_py)) {
			std::cout << str <<"P"<< std::endl;
			std::cout << "exp_px=" << exp_px << ", exp_py=" << exp_py << std::endl;
			std::cout << "px=" << p->getP().x << ", py=" << p->getP().y << std::endl << std::endl;
		}
	}

	void SignVIsRight(double exp_vx, double exp_vy) {
		if (sign(p->getV().x) != sign(exp_vx) || sign(p->getV().y) != sign(exp_vy)) {
			std::cout << str << "V" << std::endl;
			std::cout << "exp_vx=" << exp_vx << ", exp_vy=" << exp_vy << std::endl;
			std::cout << "vx=" << p->getV().x << ", vy=" << p->getV().y << std::endl << std::endl;
		}
	}

	void SignCIsRight(double exp_cx, double exp_cy) {
		if (sign(p->getC().x) != sign(exp_cx) || sign(p->getC().y) != sign(exp_cy)) {
			std::cout << str << "C" << std::endl;
			std::cout << "exp_cx=" << exp_cx << ", exp_cy=" << exp_cy << std::endl;
			std::cout << "cx=" << p->getC().x << ", cy=" << p->getC().y << std::endl << std::endl;
		}
	}

	void SetExp(int i) {
		exp_px = p0*cos(-k / l*i*dt);
		exp_py = p0*sin(-k / l*i*dt);
		exp_vx = k*p0*cos(-k / l*i*dt);
		exp_vy = k*p0*sin(-k / l*i*dt);
		exp_cx = -l*p0*sin(-k / l*i*dt);
		exp_cy = l*p0*cos(-k / l*i*dt);
	}

	~TestOscillation() {
		if (gr != NULL) delete gr;
		if (p != NULL) delete p;
	}

};

TEST_F(TestOscillation, oscillation_q_is_negative) {
	SetVar(1, constants::m, constants::q, constants::m);
	for (int i = 0; i < n; i++) {

		SetCurrentSpeedAndPos(*gr, *p, dt);

		SetExp(i);

		ASSERT_NEAR(exp_px, p->getP().x, er);
		ASSERT_NEAR(exp_py, p->getP().y, er);
		//SignPIsRight(exp_px, exp_py);

		ASSERT_NEAR(exp_vx, p->getV().x, er);
		ASSERT_NEAR(exp_vy, p->getV().y, er);
		//SignVIsRight(exp_vx, exp_vy);

		ASSERT_NEAR(exp_cx, p->getC().x, er);
		ASSERT_NEAR(exp_cy, p->getC().y, er);
		//SignCIsRight(exp_vx, exp_vy);
	}

}

TEST_F(TestOscillation, oscillation_q_is_positive) 
{
	SetVar(1, constants::m, -constants::q, constants::m);
	for (int i = 0; i < n; i++) {

		SetCurrentSpeedAndPos(*gr, *p, dt);

		SetExp(i);

		ASSERT_NEAR(exp_px, p->getP().x, er);
		ASSERT_NEAR(exp_py, p->getP().y, er);
		//SignPIsRight(exp_px, exp_py);

		ASSERT_NEAR(exp_vx, p->getV().x, er);
		ASSERT_NEAR(exp_vy, p->getV().y, er);
		//SignVIsRight(exp_vx, exp_vy);

		ASSERT_NEAR(exp_cx, p->getC().x, er);
		ASSERT_NEAR(exp_cy, p->getC().y, er);
		//SignCIsRight(exp_cx, exp_cy);
	}
}
