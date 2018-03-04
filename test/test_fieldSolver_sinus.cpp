#include "gtest.h"
#include "field_solver.h"
#include "constants.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include "grid3d.h"
#include "fourier_transformation.h"
#include "class_member_ptr.h"

static std::ofstream file;
static const std::string strE = "../../files/field_solver_test_E/";
static const std::string strB = "../../files/field_solver_test_B/";

const int a = 0, b = 100, n = 4;
const int maxIt = 8+1;
const int itTransform = 1;

class TestSinus :public testing::Test {
public:
	double dt;
	Grid3d gr;

	TestSinus() :gr(n, n, n, a, b, a, b, a, b) {
		dt = gr.gdx() / (constants::c*sqrt(2) * 8);
	}

	void SetEB(Field fE, Field fB, Coords main) {
		vec3<double> E, B;

		switch (fE) {
		case Ex: E = vec3<double>(1, 0, 0); break;
		case Ey: E = vec3<double>(0, 1, 0); break;
		case Ez: E = vec3<double>(0, 0, 1); break;
		}
		switch (fB) {
		case Bx: B = vec3<double>(1, 0, 0); break;
		case By: B = vec3<double>(0, 1, 0); break;
		case Bz: B = vec3<double>(0, 0, 1); break;
		}
		int i, j, k;
		int* ind = (main == x) ? &i : ((main == y) ? &j : &k);

		for (i = 0; i <= n; i++)
			for (j = 0; j <= n; j++)
				for (k = 0; k <= n; k++) {
					double s = sin(2 * constants::pi*(*ind) / n);
					gr(i, j, k).E = E*s;
					gr(i, j, k).B = B*s;
				}

		FourierTransformation(gr, RtoC);
	}

	double f(double x, double t) {
		return sin(2 * constants::pi / b*(constants::c*t + x));
	}

	void MyTestBodyCheckOnNull(Field fE, Field fB, Coords mainC) {
		SetEB(fE, fB, mainC);

		for (int j = 0; j < maxIt; j++) {
			FieldSolver(gr, j*dt);
			if (j%itTransform == 0) {
				FourierTransformation(gr, CtoR);
			}

			Coords othCoordB1 = (fB + 1) % 3, othCoordB2 = (fB + 2) % 3;
			Coords othCoordE1 = (fE + 1) % 3, othCoordE2 = (fE + 2) % 3;

			int ix=0, iy=0, iz=0;

			for (int* i = (mainC==x?&ix:(mainC==y?&iy:&iz)); (*i) <= gr.gnxReal(); (*i)++) {
				EXPECT_NEAR(0, (gr(ix, iy, iz).*GetField(B).*GetCoord(othCoordB1))(), 1E-5);
				EXPECT_NEAR(0, (gr(ix, iy, iz).*GetField(B).*GetCoord(othCoordB2))(), 1E-5);
				EXPECT_NEAR(0, (gr(ix, iy, iz).*GetField(E).*GetCoord(othCoordE1))(), 1E-5);
				EXPECT_NEAR(0, (gr(ix, iy, iz).*GetField(E).*GetCoord(othCoordE2))(), 1E-5);
			}
			

			WriteFile(B, othCoordB1, mainC, j, strB + "test_zeros_");
			WriteFile(B, othCoordB2, mainC, j, strB + "test_zeros_");
			WriteFile(E, othCoordE1, mainC, j, strE + "test_zeros_");
			WriteFile(E, othCoordE2, mainC, j, strE + "test_zeros_");
		}
	}

	void WriteFile(Field field, Coords fieldCoord, Coords iterC, int j, std::string name) {
		file.open(name + "iter_" + std::to_string(j) +"_coord_"+std::to_string(fieldCoord)+ ".xls");//freopen?

		int ix = 0, iy = 0, iz = 0;

		for (int* i = (iterC == x ? &ix : (iterC == y ? &iy : &iz)); (*i) <= gr.gnxReal(); (*i)++) {
			file << (double)(gr(ix, iy, iz).*GetField(field).*GetCoord(fieldCoord))() << std::endl;
		}
		file.close();
	}

	void MyTestBodyWriteFile(Field fE, Field fB, Coords mainC) {
		SetEB(fE, fB, mainC);

		for (int j = 0; j < maxIt; j++) {
			FieldSolver(gr, j*dt);

			if (j%itTransform == 0) {
				FourierTransformation(gr, CtoR);

				WriteFile(E, fE, mainC, j, strE);
				WriteFile(B, fB, mainC, j, strB);

				//std::cout << "#############" << j << std::endl;
				for (int i = 0; i <= n; i++)
				{
					ASSERT_NEAR(f(i, j*dt), gr(i, 0, 0).B.z(), 1E-2 * (j + 1)*(j + 1));
					ASSERT_NEAR(f(i, j*dt), gr(i, 0, 0).E.y(), 1E-2 * (j + 1)*(j + 1));
				}
			}
		}

	}

	~TestSinus() {}
};


TEST_F(TestSinus, other_components_is_null_Ey_Bz) {
	MyTestBodyCheckOnNull(Ey, Bz, x);
}

TEST_F(TestSinus, res_is_sinus_Ey_Bz) {
	MyTestBodyWriteFile(Ey, Bz, x);
}
/*
TEST_F(TestSinus, other_components_is_null_Ez_By) {
	SetEB(3, 2, 1);
	for (int j = 0; j < maxIt; j++) {
		FieldSolver(gr, j*dt);
		FourierTransformation(gr, CtoR);

		for (int i = 0; i <= gr.gnxReal(); i++) {
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

	}
}

TEST_F(TestSinus, other_components_is_null_Ez_Bx) {
	SetEB(3, 1,2);
	for (int j = 0; j < maxIt; j++) {
		FieldSolver(gr, j*dt);
		FourierTransformation(gr, CtoR);

		for (int i = 0; i <= gr.gnyReal(); i++) {
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


	}
}*/