#include "gtest.h"
#include "grid3d.h"
#include "fourier_transformation.h"
#include "constants.h"
#include "class_member_ptr.h"
#include "my_complex.h"
#include "fftw3.h"

class TestFourierTransform : public testing::Test {
public:
	int nx = 10, ny = 5, nz = 3;
	double X = 6, Y = 8, Z = 10;
	Grid3d grid;

	TestFourierTransform() : grid(nx, ny, nz, 0, X, 0, Y, 0, Z) {
		double A = 10;
		for (int i = 0; i < nx; i++)
			for (int j = 0; j < ny; j++)
				for (int k = 0; k < nz; k++) {
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

		for (int i = 0; i < grid.gnxRealCells(); i++)
			for (int j = 0; j < grid.gnyRealCells(); j++)
				for (int k = 0; k < grid.gnzRealCells(); k++)
					ASSERT_NEAR((grid(i, j, k).*p.*m)(), (grid2(i, j, k).*p.*m)(), 0.1);
	}
};

TEST_F(TestFourierTransform, no_throws_RtoC) {
	ASSERT_NO_THROW(FourierTransformation(grid, Ex, RtoC));
}

TEST_F(TestFourierTransform, no_throws_CtoR) {
	ASSERT_NO_THROW(FourierTransformation(grid, Ex, CtoR));
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


TEST_F(TestFourierTransform, fourier_transform_writes_data_correctly_to_grid) {
	Array3d<MyComplex> arr1(grid.gnxRealCells(),grid.gnyRealCells(),grid.gnzRealCells()), arr2(grid.gnxRealCells(),grid.gnyRealCells(),grid.gnzRealCells());
	for (int i = 0; i < arr1.gnx(); i++)
		for (int j = 0; j < arr1.gny(); j++)
			for (int k = 0; k < arr1.gnz(); k++)
				arr1(i,j,k).SetReal(grid(i, j, k).E.x());

	fftw_plan plan = fftw_plan_dft_3d(grid.gnxRealCells(), grid.gnyRealCells(), grid.gnzRealCells(), (fftw_complex*)&(arr1[0]), (fftw_complex*)&(arr2[0]), FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(plan);

	FourierTransformation(grid, Ex, RtoC);

	for (int i = 0; i < grid.gnxComplexCells(); i++)
		for (int j = 0; j < grid.gnyComplexCells(); j++)
			for (int k = 0; k < grid.gnzComplexCells(); k++) {
				ASSERT_EQ(grid(i, j, k).EF[0], arr2(i, j, k));
			}

}