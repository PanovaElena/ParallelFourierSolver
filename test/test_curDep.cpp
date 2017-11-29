#include "gtest.h"
#include "current_deposition.h"
#include "grid2d.h"
#include "particle.h"
#include <cstdlib>
#include <ctime>

TEST(test_current_deposition, can_find_I_in_complex_grid) 
{
	int nx = 4, ny = 4;
	Grid2d gr(nx, ny, 8, 8);

	double x = 6.3, y = 2.7;
	Particle p(vec3(x, y, 0), vec3(10,3.2,0), 4, 0);

	CurrentDeposition(gr, p);

	vec3 I_exp = getJ(p, gr.gdx(), gr.gdy());
	vec3 I = gr(3, 1).J+ gr(3, 2).J + gr(4, 1).J + gr(4, 2).J;

	EXPECT_DOUBLE_EQ(I_exp.x, I.x);
	EXPECT_DOUBLE_EQ(I_exp.y, I.y);
	EXPECT_DOUBLE_EQ(I_exp.z, I.z);
}

TEST(test_current_deposition, can_distribute_I_correctly)
{
	int nx = 4, ny = 4, dx=2, dy=2;
	Grid2d gr(nx, ny, 8, 8);

	double x = 6.3, y = 2.7;//s(0,0)=0.3*0.7; s(0,1)=0.3*1.3; s(1,0)=1.7*0.7; s(1,1)=1.7*1.3
	Particle p(vec3(x, y, 0), vec3(10, 3.2, 0), 4, 0);

	CurrentDeposition(gr, p);

	vec3 I[2][2];
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			I[i][j] = gr(3 + i, 1 + j).J;

	vec3 J = getJ(p, gr.gdx(), gr.gdy());
	double s[2][2] = { { 0.3*0.7/(dx*dy) , 0.3*1.3 / (dx*dy) }, { 1.7*0.7 / (dx*dy), 1.7*1.3 / (dx*dy) } };
	int i = 3, j = 1;


	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++) {
			EXPECT_NEAR(gr(3 + i, 1 + j).J.x, J.x*s[1 - i][1 - j], 1E-10);
			EXPECT_NEAR(gr(3 + i, 1 + j).J.y, J.y*s[1 - i][1 - j], 1E-10);
			EXPECT_NEAR(gr(3 + i, 1 + j).J.z, J.z*s[1 - i][1 - j], 1E-10);
		}

}

TEST(test_current_deposition, can_find_sum) {
	int nx = 10, ny = 20, ax = 5, ay = 15, bx = 10, by = 30;

	//main grid
	Grid2d gr(nx, ny, ax, ay, bx, by);

	const int N = 10;
	std::vector<Particle> p(N);
	//srand(time(0));
	srand(1);
	double x, y;

	for (int i = 0; i < N; i++) {
		x= ((double)rand()) / RAND_MAX*(bx - ax) + ax;
		y = ((double)rand()) / RAND_MAX*(by - ay) + ay;
		p[i].setC(vec3(x,y,0));
	}

	CurrentDeposition(gr, p);

	//test grids
	Grid2d arrGrid[N];
	for (int i = 0; i < N; i++) {
		arrGrid[i].Initialize(nx, ny, ax, ay, bx, by);
		CurrentDeposition(arrGrid[i], p[i]);
	}
	//sum
	Grid2d exp(nx, ny, ax, ay, bx, by);
	for (int i = 0; i < N; i++)
		for (int j = 0; j <= nx; j++)
			for (int k = 0; k <= ny; k++)
				exp(j, k).J += arrGrid[i](j,k).J;
	for (int i = 0; i <= ny; i++) {
		gr(nx, i).J = gr(0, i).J += gr(nx, i).J;
	}
	for (int i = 0; i <= nx; i++) {
		gr(i, ny).J = gr(i, 0).J += gr(i, ny).J;
	}


	for (int i = 0; i <= nx; i++)
		for (int j = 0; j <= ny; j++)
			EXPECT_EQ(exp(i, j).J, gr(i, j).J);
	for (int i = 0; i <= ny; i++)
		EXPECT_EQ(gr(nx, i).J, gr(0, i).J);
	for (int i = 0; i <= nx; i++) 
		EXPECT_EQ(gr(i, ny).J, gr(i, 0).J);

}