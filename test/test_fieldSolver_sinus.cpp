#include "gtest.h"
#include "field_solver.h"
#include "constants.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include "grid3d.h"
#include "fourier_transformation.h"

static std::ofstream file;
static const std::string strE = "../../files/field_solver_test_E";
static const std::string strB = "../../files/field_solver_test_B";

const int a = 0, b = 100, n = 100;
const int maxIt = 2;

class TestSinus:public testing::Test {
public:
	double dt;
	Grid3d gr;

	TestSinus():gr(n,n,n,a,a,a,b,b,b) {
		dt = gr.gdx() / (constants::c*sqrt(2)*8);
	}

	void SetEB(int coordE, int coordB, int main) {
		vec3<double> E, B;

		switch (coordE) {
		case 1: E = vec3<double>(1, 0, 0); break;
		case 2: E = vec3<double>(0, 1, 0); break;
		case 3: E = vec3<double>(0, 0, 1); break;
		}
		switch (coordB) {
		case 1: B = vec3<double>(1, 0, 0); break;
		case 2: B = vec3<double>(0, 1, 0); break;
		case 3: B = vec3<double>(0, 0, 1); break;
		}
		int i, j, k;
		int* ind = (main == 1) ? &i : ((main == 2) ? &j : &k);

		for (i = 0; i <= n; i++)
			for (j = 0; j <= n; j++) 
				for (k = 0; k <= n; k++) {
					double s = sin(2 * constants::pi*(*ind) / b);
					gr(i, j, k).E = E*s;
					gr(i, j, k).B = B*s;
				}

		FourierTransformation(gr, RtoC);
	}

	double f(double x, double t) {
		return sin(2 * constants::pi/b*(constants::c*t+x));
	}
	~TestSinus(){}
};


TEST_F(TestSinus, other_components_is_null_Ey_Bz) {
	SetEB(2, 3, 1);
	for (int j = 0; j < maxIt; j++) {
		FieldSolver(gr, j*dt);
		FourierTransformation(gr, CtoR);

		for (int i = 0; i <= gr.gnxCells(); i++) {
			ASSERT_DOUBLE_EQ(0, gr(i, 0, 0).B.x());
			ASSERT_DOUBLE_EQ(0, gr(i, 0, 0).B.y());
			ASSERT_DOUBLE_EQ(0, gr(i, 0, 0).E.x());
			ASSERT_DOUBLE_EQ(0, gr(i, 0, 0).E.z());
		}
	}
}

TEST_F(TestSinus, res_is_sinus_Ey_Bz) {
	SetEB(2, 3,1);

	for (int j = 0; j < maxIt; j++) {
		FieldSolver(gr, j*dt);
		FourierTransformation(gr, CtoR);

		file.open(strE + "_" + to_string(j) + ".x()ls");//freopen?

		//косинусоида (кроме граничных точек), большая погрешность в области Ey=0 при большом шаге
		for (int i = 0; i <= n; i++) { 
			file << (double)gr(i, 0, 0).E.y() << std::endl;
		}
		file.close();

		file.open(strB + "_" + to_string(j) + ".x()ls");
		for (int i = 0; i <= n; i++) { 
			file << (double)gr(i, 0, 0).B.z() << std::endl;
		}
		file.close();
		//std::cout << "#############" << j << std::endl;
		/*for (int i = 0; i <= n; i++)
		{
			ASSERT_NEAR(f(i,j*dt), gr(i, 0, 0).B.z(), 1E-2 * (j + 1)*(j + 1));
			ASSERT_NEAR(f(i, j*dt), gr(i, 0, 0).E.y(), 1E-2 * (j + 1)*(j + 1));
		}*/
	}
}

TEST_F(TestSinus, other_components_is_null_Ez_By) {
	SetEB(3, 2, 1);
	for (int j = 0; j < maxIt; j++) {
		FieldSolver(gr, j*dt);
		FourierTransformation(gr, CtoR);

		for (int i = 0; i <= gr.gnxCells(); i++) {
			ASSERT_DOUBLE_EQ(0, gr(i, 0, 0).B.x());
			ASSERT_DOUBLE_EQ(0, gr(i, 0, 0).B.z());
			ASSERT_DOUBLE_EQ(0, gr(i, 0, 0).E.y());
			ASSERT_DOUBLE_EQ(0, gr(i, 0, 0).E.x());
		}
	}
}

TEST_F(TestSinus, res_is_sinus_Ez_By) {
	SetEB(3, 2, 1);

	for (int j = 0; j < maxIt; j++) {
		FieldSolver(gr, j*dt);
		FourierTransformation(gr, CtoR);

		//std::cout << "#############" << j << std::endl;
		/*for (int i = 0; i <= n; i++)
		{
			//экспериментально: глобальная погрешность 0.01 на 0 шаге, увеличивается при последующих шагах
			ASSERT_NEAR(f(i, j*dt), gr(i, 0).B.y(), 1E-2 * (j + 1)*(j + 1));
			ASSERT_NEAR(f(i, j*dt), gr(i, 0).E.z(), 1E-2 * (j + 1)*(j + 1));
		}*/
	}
}

TEST_F(TestSinus, other_components_is_null_Ez_Bx) {
	SetEB(3, 1,2);
	for (int j = 0; j < maxIt; j++) {
		FieldSolver(gr, j*dt);
		FourierTransformation(gr, CtoR);

		for (int i = 0; i <= gr.gnyCells(); i++) {
			ASSERT_DOUBLE_EQ(0, gr(0, i, 0).B.y());
			ASSERT_DOUBLE_EQ(0, gr(0, i, 0).B.z());
			ASSERT_DOUBLE_EQ(0, gr(0, i, 0).E.x());
			ASSERT_DOUBLE_EQ(0, gr(0, i, 0).E.y());
		}
	}
}

TEST_F(TestSinus, res_is_sinus_Ez_Bx) {
	SetEB(3, 1,2);

	for (int j = 0; j < maxIt; j++) {
		FieldSolver(gr, j*dt);
		FourierTransformation(gr, CtoR);

		//std::cout << "#############" << j << std::endl;
		/*for (int k = 0; k <= n; k++) {
			//экспериментально: глобальная погрешность 0.01 на 0 шаге, увеличивается при последующих шагах
			ASSERT_NEAR(f(k, j*dt), gr(0, k).B.x(), 1E-2 * (j + 1)*(j + 1));
			ASSERT_NEAR(f(k, j*dt), gr(0, k).E.z(), 1E-2 * (j + 1)*(j + 1));
		}*/
	}
}