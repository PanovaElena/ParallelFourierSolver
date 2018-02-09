#include "gtest.h"
#include "grid3d.h"
#include "fourier_transformation.h"
#include "constants.h"
#include "class_member_ptr.h"

class TestFourierTransform : public testing::Test {
public:
	int nx = 4, ny = 5, nz = 6;
	double X = 6, Y = 8, Z = 10;
	Grid3d grid;

	TestFourierTransform() : grid(nx, ny, nz, 0, X, 0, Y, 0, Z) {
		double A = 10;
		for (int i = 0; i <= nx; i++)
			for (int j = 0; j <= ny; j++)
				for (int k = 0; k <= nz; k++) {
					double x = A * sin(2 * constants::pi*k / nx);
					double y = A * sin(2 * constants::pi*i / nz);
					double z = A * sin(2 * constants::pi*j / ny);

					grid(i, j, k).E[0] = grid(i, j, k).B[0] = x;
					grid(i, j, k).E[1] = grid(i, j, k).B[1] = y;
					grid(i, j, k).E[2] = grid(i, j, k).B[2] = z;
				}
	}

	
 
	void MyTestBody(Field field) {

		MemberOfNode p = GetField(field/3);
		MethodCoord m= GetCoord(field % 3);

		Grid3d grid2 = grid;

		FourierTransformation(grid, field, RtoC);
		FourierTransformation(grid, field, CtoR);

		for (int i = 0; i <= grid.gnxCells(); i++)
			for (int j = 0; j <= grid.gnyCells(); j++)
				for (int k = 0; k <= grid.gnzCells(); k++)
					ASSERT_NEAR((grid(i, j, k).*p.*m)(), (grid2(i, j, k).*p.*m)(), 0.1);
	}
};

TEST_F(TestFourierTransform, no_throws) {
	ASSERT_NO_THROW(FourierTransformation(grid, Ex, RtoC));
}

TEST_F(TestFourierTransform, transform_correctly_Ex) {
	MyTestBody(Ex);
}

TEST_F(TestFourierTransform, transform_correctly_Ey) {
	MyTestBody(Ey);
}

TEST_F(TestFourierTransform, transform_correctly_Ez) {
	MyTestBody(Ez);
}

TEST_F(TestFourierTransform, transform_correctly_Bx) {
	MyTestBody(Bx);
}

TEST_F(TestFourierTransform, transform_correctly_By) {
	MyTestBody(By);
}

TEST_F(TestFourierTransform, transform_correctly_Bz) {
	MyTestBody(Bz);
}