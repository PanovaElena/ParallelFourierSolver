#include <gtest.h>
#include "particle_push.h"
#include "constants.h"
#include "particle.h"
#include "grid2d.h"
#include <iostream>
#include <fstream>
using std::cout;
Grid2d getStartGr(int nx, int ny,  double ax, double ay, double bx, double by, vec3 B, vec3 E)
{
	return Grid2d(nx, ny, ax, ay, bx, by, B, E);
}
class TestFindSpeed : public ::testing::Test
{
private:
	inline double sign(double x)
	{
		if (x >= 0.0) return 1.0;
		return -1.0;
	}
public:
	int nx = 4;
	int ny = 4;
	int testFactor = 1;
	Particle part;
	void setFactor(int factor)
	{
		testFactor = factor;
		part.setFactor(factor);
	}
	void NEAR(double a, double b)
	{
		EXPECT_NEAR(sign(a / b)*(a / b), 1.0, 0.078);
	}
	void SetUp()
	{
		vec3 c(0, 0, 0), v(0, 0, 0);
		part = Particle(c, v, constants::q, constants::m, testFactor);
	}
	double getDtEl(double e, int numRep)
	{
		return (constants::c * part.getM()) / (abs(part.getQ() * e) * numRep);
	}
	double getDtMg(double p, double b, int numRep)
	{
		return  constants::pi*constants::speedLight*
			part.getM()*sqrt(1 + p*p/(part.getM()*part.getM() *constants::speedLightSQ)) / (abs(part.getQ() * b)*numRep);
	}
	double getRVecEl(double e)
	{
		double sq = sqrt(2.0) - 1;
		return  part.getM()*constants::speedLightSQ*sq /(part.getQ()*e);
	}
	double getPVecEl(double e)// e - значение поля
	{
		return  sign(e*part.getQ())*part.getM() * constants::c;
	}
	double getRVecMg(double p, double b)
	{
		return -2.0 * p * constants::speedLight / (b * part.getQ());
	}
	Particle getPartEl(double e, vec3 E, double ax, double ay, double bx, double by, int numRep)
	{
		Grid2d gr = getStartGr(nx, ny, ax, ay, bx, by, vec3(0, 0, 0), E);
		double dt = getDtEl(e, numRep);
		for (int i = 0; i < numRep; i++)
		{
			if (!gr.inside(part.getC()))
			{
				throw "particle not inside!";
			}
			SetCurrentSpeedAndPos(gr, part, dt);
		}
		return part;
	}
	Particle getPartMg(double b, vec3 B, double p, vec3 Pulse, double ax, double ay, double bx, double by, int numRep)
	{
		part.setP(Pulse);
		Grid2d gr = getStartGr(nx, ny, ax, ay, bx, by, B, vec3(0, 0, 0));
		double dt = getDtMg(p, b, numRep);
		for (int i = 0; i < numRep; i++)
		{
			if (!gr.inside(part.getC())) throw "particle not inside!";
			SetCurrentSpeedAndPos(gr, part, dt);
		}
		return part;
	}
};
TEST_F(TestFindSpeed, electr_pulse_and_position1)
{
	double ex = 100.0;
	Particle p = getPartEl(ex, vec3(ex, 0, 0), -100000.0, -10.0, 10000.0, 10.0, 10);
	double ansp = getPVecEl(ex);
	EXPECT_NEAR(p.getP().x, ansp, 1e-15);
	EXPECT_DOUBLE_EQ(p.getP().y, 0);
	EXPECT_DOUBLE_EQ(p.getP().z, 0);
	NEAR(p.getP().x, ansp);
	double pos = getRVecEl(ex);
	NEAR(pos, p.getC().x);
}
TEST_F(TestFindSpeed, electr_pulse_and_position2)
{
	double ex = 1000.0;
	Particle p = getPartEl(ex, vec3(ex, 0, 0), -1000000.0, -10.0, 1000000.0, 10.0, 10000);
	double ansp = getPVecEl(ex);
	EXPECT_NEAR(p.getP().x, ansp, 1e-15);
	EXPECT_DOUBLE_EQ(p.getP().y, 0);
	EXPECT_DOUBLE_EQ(p.getP().z, 0);
	NEAR(p.getP().x, ansp);
	double pos = getRVecEl(ex);
	NEAR(pos, p.getC().x);
}
TEST_F(TestFindSpeed, electr_pulse_and_position1_y)
{
	double ey = 100.0;
	Particle p = getPartEl(ey, vec3(0, ey, 0), -10000.0, -100000000.0, 10000.0, 10000000000.0, 10000);
	double ansp = getPVecEl(ey);
	EXPECT_NEAR(p.getP().y, ansp, 1e-25);
	EXPECT_DOUBLE_EQ(p.getP().x, 0);
	EXPECT_DOUBLE_EQ(p.getP().z, 0);
	NEAR(p.getP().y, ansp);
	double pos = getRVecEl(ey);
	NEAR(pos, p.getC().y);
}
TEST_F(TestFindSpeed, electr_pulse_position2_y)
{
	setFactor(100);
	double ey = 1000.0;
	Particle p = getPartEl(ey, vec3(0, ey, 0), -10000000000.0, -1000000.0, 10000.0, 10000000000.0, 10000);
	double ansp = getPVecEl(ey);
	EXPECT_NEAR(p.getP().y, ansp, 1e-17);
	EXPECT_DOUBLE_EQ(p.getP().x, 0);
	EXPECT_DOUBLE_EQ(p.getP().z, 0);
	NEAR(p.getP().y, ansp);
	double pos = getRVecEl(ey);
	NEAR(pos, p.getC().y);
}
TEST_F(TestFindSpeed, magn_pulse_position1)
{
	double p = 1e-35;
	double b = 1.0;
	vec3 Pulse(p, 0.0, 0.0);
	vec3 B(0, 0, b);
	getPartMg(b, B, p, Pulse, -2000.0, -2000.0, 10.0, 10.0, 1000);
	EXPECT_NEAR(p, -part.getP().x, 1e-40);
	EXPECT_NEAR(part.getC().y, getRVecMg(p, b), 1e-20);
}
TEST_F(TestFindSpeed, magn_pulse_position2)
{
	double p = 1e-40;
	double b = 1.0;
	vec3 Pulse(p, 0.0, 0.0);
	vec3 B(0, 0, b);
	getPartMg(b, B, p, Pulse, -200000.0, -200000.0, 100.0, 100.0, 100000);
	EXPECT_NEAR(p, -part.getP().x, 1e-45);
	EXPECT_NEAR(part.getC().y, getRVecMg(p, b), 1e-25);
}
TEST_F(TestFindSpeed, magn_pulse_position3)
{
	double p = constants::m*testFactor;
	double b = 1.0;
	vec3 Pulse(p, 0.0, 0.0);
	vec3 B(0, 0, b);
	getPartMg(b, B, p, Pulse, -200000.0, -200000.0, 100.0, 100.0, 100000);
	EXPECT_NEAR(p, -part.getP().x, 1e-35);
	EXPECT_NEAR(part.getC().y, getRVecMg(p, b), 1e-16);
}
TEST_F(TestFindSpeed, magn_pulse_position4)
{
	double p = 1e-30 * testFactor;
	double b = 1.0;
	vec3 Pulse(p, 0.0, 0.0);
	vec3 B(0, 0, b);
	getPartMg(b, B, p, Pulse, -200000.0, -200000.0, 100.0, 100.0, 100000);
	EXPECT_NEAR(p, -part.getP().x, 1e-35);
	EXPECT_NEAR(part.getC().y, getRVecMg(p, b), 1e-17);
}
/*TEST_F(TestFindSpeed, magn_pulse_position_print)
{
	double p = 1e-21;
	vec3 Pulse(p, 0.0, 0.0);
	part.setP(Pulse);

	double b = 0.1;
	vec3 B(0, 0, b);

	double dt = 2e-28;
	int nr = 100;
	Grid2d gr = getStartGr(nx, ny, -200.0, -200.0, 100.0, 100.0, B, vec3(0, 0, 0));
	std::ofstream fout("output5.txt");
    fout << part.getC().x << ' ' << part.getC().y << '\n';
	for (int i = 0; i < nr; i++)
	{
		if (!gr.inside(part.getC())) throw - 1;
		{
			SetCurrentSpeedAndPos(gr, part, dt);
			fout << part.getC().x << ' ' << part.getC().y << '\n';
		}
	}
	fout.close();
}*/
TEST_F(TestFindSpeed, saved_speed)
{
	Grid2d gr = getStartGr(4, 4, 0.0, 0.0, 40.0, 40.0, vec3(0.0,0.0,0.0), vec3(0.0,0.0,0.0));
	part.setV(vec3(10.0, 10.0, 0.0));
	double dt = 0.01;
	int numRep = 10;
	for (int i = 0; i < numRep; i++)
		SetCurrentSpeedAndPos(gr, part, dt);
	EXPECT_DOUBLE_EQ(10.0, part.getV().x);
	EXPECT_DOUBLE_EQ(10.0, part.getV().y);
	EXPECT_DOUBLE_EQ(0.0, part.getV().z);

	EXPECT_DOUBLE_EQ(1.0, part.getC().x);
	EXPECT_DOUBLE_EQ(1.0, part.getC().y);
	EXPECT_DOUBLE_EQ(0.0, part.getC().z);
}
TEST_F(TestFindSpeed, inside_workspace)
{
	Grid2d gr = getStartGr(4, 4, 0.0, 0.0, 40.0, 40.0, vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0));
	part.setV(vec3(10.0, -10.0, 0.0));
	double dt = 0.1;
	int numRep = 1 + 40;
	for (int i = 0; i < numRep; i++)
	{
		SetCurrentSpeedAndPos(gr, part, dt);
		ASSERT_TRUE(gr.inside(part.getC()));
	}
	EXPECT_DOUBLE_EQ(10.0, part.getV().x);
	EXPECT_DOUBLE_EQ(-10.0, part.getV().y);
	EXPECT_DOUBLE_EQ(0.0, part.getV().z);

	EXPECT_DOUBLE_EQ(1.0, part.getC().x);
	EXPECT_DOUBLE_EQ(39.0, part.getC().y);
	EXPECT_DOUBLE_EQ(0.0, part.getC().z);
}

