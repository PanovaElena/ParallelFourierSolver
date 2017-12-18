#include "gtest.h"
#include "grid3d.h"
#include "fourier_transformation.h"
#include "constants.h"

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
					double x = A * sin(2 * constants::pi*i / nx);
					double y = A * sin(2 * constants::pi*i / nx);
					double z = A * sin(2 * constants::pi*i / nx);

					grid(i, j, k).E[0] = grid(i, j, k).B[0] = x;
					grid(i, j, k).E[1] = grid(i, j, k).B[1] = y;
					grid(i, j, k).E[2] = grid(i, j, k).B[2] = z;
				}
	}
};

TEST_F(TestFourierTransform, no_throws) {
	ASSERT_NO_THROW(FourierTransformation(grid, Ex, RtoC));
}

TEST_F(TestFourierTransform, transform_correctly_Ex) {
	Grid3d grid2 = grid;

	FourierTransformation(grid, Ex, RtoC);
	FourierTransformation(grid, Ex, CtoR);

	for (int i = 0; i <= grid.gnxCells(); i++)
		for (int j = 0; j <= grid.gnyCells(); j++)
			for (int k = 0; k <= grid.gnzCells(); k++)
				ASSERT_NEAR(grid(i, j, k).E.x(), grid2(i, j, k).E.x(), 0.1);
}

TEST_F(TestFourierTransform, transform_correctly_Ey) {
	Grid3d grid2 = grid;
	FourierTransformation(grid, Ey, RtoC);
	FourierTransformation(grid, Ey, CtoR);

	for (int i = 0; i <= grid.gnxCells(); i++)
		for (int j = 0; j <= grid.gnyCells(); j++)
			for (int k = 0; k <= grid.gnzCells(); k++)
				ASSERT_NEAR(grid(i, j, k).E.y(), grid2(i, j, k).E.y(), 0.1);
}

TEST_F(TestFourierTransform, transform_correctly_Ez) {
	Grid3d grid2 = grid;
	FourierTransformation(grid, Ez, RtoC);
	FourierTransformation(grid, Ez, CtoR);

	for (int i = 0; i <= grid.gnxCells(); i++)
		for (int j = 0; j <= grid.gnyCells(); j++)
			for (int k = 0; k <= grid.gnzCells(); k++)
				ASSERT_NEAR(grid(i, j, k).E.z(), grid2(i, j, k).E.z(), 0.1);
}

TEST_F(TestFourierTransform, transform_correctly_Bx) {
	Grid3d grid2 = grid;
	FourierTransformation(grid, Bx, RtoC);
	FourierTransformation(grid, Bx, CtoR);

	for (int i = 0; i <= grid.gnxCells(); i++)
		for (int j = 0; j <= grid.gnyCells(); j++)
			for (int k = 0; k <= grid.gnzCells(); k++)
				ASSERT_NEAR(grid(i, j, k).B.x(), grid2(i, j, k).B.x(), 0.1);
}

TEST_F(TestFourierTransform, transform_correctly_By) {
	Grid3d grid2 = grid;
	FourierTransformation(grid, By, RtoC);
	FourierTransformation(grid, By, CtoR);

	for (int i = 0; i <= grid.gnxCells(); i++)
		for (int j = 0; j <= grid.gnyCells(); j++)
			for (int k = 0; k <= grid.gnzCells(); k++)
				ASSERT_NEAR(grid(i, j, k).B.y(), grid2(i, j, k).B.y(), 0.1);
}

TEST_F(TestFourierTransform, transform_correctly_Bz) {
	Grid3d grid2 = grid;
	FourierTransformation(grid, Bz, RtoC);
	FourierTransformation(grid, Bz, CtoR);

	for (int i = 0; i <= grid.gnxCells(); i++)
		for (int j = 0; j <= grid.gnyCells(); j++)
			for (int k = 0; k <= grid.gnzCells(); k++)
				ASSERT_NEAR(grid(i, j, k).B.z(), grid2(i, j, k).B.z(), 0.1);
}