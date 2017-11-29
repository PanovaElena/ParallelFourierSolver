#include "gtest.h"
#include "field_interpolation.h"
#include "particle.h"
#include "grid2d.h"


TEST(test_work, I_work1)
{
	EXPECT_NO_THROW(Grid2d n);
}

TEST(test_find_index, find_index)
{
	int nx = 4, ny = 4;
	int size = 8;
	Grid2d gr(nx, ny, size, size);
	//если в конце, то в начало
	for (int i = 0; i < nx; i++)
		for (int j = 0; j < ny; j++)
		{
			auto p = Particle(vec3(i*size /nx, j*size /ny, 0), vec3(0, 0, 0), 0, 0);
			EXPECT_EQ(gr.getIndex(p.getC().x, p.getC().y).x, i%nx);
			EXPECT_EQ(gr.getIndex(p.getC().x, p.getC().y).y, j%ny);
		}
	auto p = Particle(vec3((size / nx) - 0.001, size / ny, 0), vec3(0, 0, 0), 0, 0);
	EXPECT_EQ(gr.getIndex(p.getC().x, p.getC().y).x, 0);
	EXPECT_EQ(gr.getIndex(p.getC().x, p.getC().y).y, 1);
}


TEST(test_particle, find_E_in_particle)
{
	int nx = 1, ny = 1;
	Grid2d gr(nx,ny,1,1);
	gr.getRefNode(0, 0).E = vec3(1, 1,0);
	gr.getRefNode(0, 1).E = vec3(-1, 1, 0);
	gr.getRefNode(1, 0).E = vec3(1, -1, 0);
	gr.getRefNode(1, 1).E = vec3(-1, -1, 0);

	Particle p(vec3(0.5,0.5,0),vec3(), 0, 0);
	vec3 E = calcE(gr,p);
	EXPECT_EQ(E.x, 0);
	EXPECT_EQ(E.y, 0);
	EXPECT_EQ(E.z, 0);
}

TEST(test_particle, find_E_in_complex_grwork)
{
	int nx = 4, ny = 4;
	Grid2d gr(nx, ny, 8, 8);
	double dx = gr.gdx(), dy = gr.gdy();
	for (int i = 0; i <= nx; i++)
		for (int j = 0; j <= ny; j++)
			gr.getRefNode(i,j).E = vec3(i*dx+j*dy, i*dx - j*dy, 0);

	double x = 6.3, y = 2.7;
	Particle p(vec3(x, y, 0), vec3(), 0, 0);
	vec3 E = calcE(gr,p);
	EXPECT_DOUBLE_EQ(E.x, x+y);
	EXPECT_DOUBLE_EQ(E.y, x-y);
	EXPECT_DOUBLE_EQ(E.z, 0);
}