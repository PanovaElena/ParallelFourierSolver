#include "gtest.h"
#include "grid3d.h"
#include "fourier_transformation.h"
#include "constants.h"

class TestFourierTransform : public testing::Test {
public:
	int nx = 3, ny = 4, nz = 5;
	double X = 6, Y = 8, Z = 10;
	Grid3d grid;

	TestFourierTransform(): grid(nx,ny,nz,0,X,0,Y,0,Z) {
		for (int i=0; i<=nx; i++)
			for (int j = 0; j <= ny; j++)
				for (int k = 0; k <= nz; k++) {
					double x = sin(2 * constants::pi*i / X);
					double y = sin(2 * constants::pi*j / Y);
					double z = sin(2 * constants::pi*k / Z);

					grid(i, j, k).E.x = grid(i, j, k).B.x = x;
					grid(i, j, k).E.y = grid(i, j, k).B.y = y;
					grid(i, j, k).E.z = grid(i, j, k).B.z = z;
				}
	}
};

TEST_F(TestFourierTransform, transform_correctly_E) {
	Grid3d grid2 = grid;
	FourierTransformation(grid, E, RtoC);
	FourierTransformation(grid, E, CtoR);

	EXPECT_TRUE(grid2==grid);
}