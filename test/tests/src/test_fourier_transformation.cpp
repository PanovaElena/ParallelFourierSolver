#include "gtest.h"
#include "grid3d.h"
#include "fourier_transformation.h"
#include "physical_constants.h"
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

                    grid.E[0](i, j, k) = grid.B[0](i, j, k) = x;
                    grid.E[1](i, j, k) = grid.B[1](i, j, k) = y;
                    grid.E[2](i, j, k) = grid.B[2](i, j, k) = z;
                }
    }

    
 
    void MyTestBody(Field field, Coordinate coord) {

        MemberOfNode p = GetField<double>(field);
        MemberOfField m = GetFieldCoord<double>(coord);

        Grid3d grid2 = grid;
         
        FourierTransformation(grid, field, coord, RtoC);
        FourierTransformation(grid, field, coord, CtoR);

		for (int i = 0; i < grid.gnxRealCells(); i++)
			for (int j = 0; j < grid.gnyRealCells(); j++)
				for (int k = 0; k < grid.gnzRealCells(); k++)
					ASSERT_NEAR((grid.*p.*m)(i, j, k), (grid2.*p.*m)(i, j, k), 0.1);
	}
};

TEST_F(TestFourierTransform, no_throws_RtoC) {
    ASSERT_NO_THROW(FourierTransformation(grid, E, x, RtoC));
}

TEST_F(TestFourierTransform, no_throws_CtoR) {
    ASSERT_NO_THROW(FourierTransformation(grid, E, x, CtoR));
}

TEST_F(TestFourierTransform, transform_correctly_Ex) {
    MyTestBody(E, x);
}

TEST_F(TestFourierTransform, transform_correctly_Ey) {
    MyTestBody(E, y);
}

TEST_F(TestFourierTransform, transform_correctly_Ez) {
    MyTestBody(E, z);
}

TEST_F(TestFourierTransform, transform_correctly_Bx) {
    MyTestBody(B, x);
}

TEST_F(TestFourierTransform, transform_correctly_By) {
    MyTestBody(B, y);
}

TEST_F(TestFourierTransform, transform_correctly_Bz) {
    MyTestBody(B, z);
}


TEST_F(TestFourierTransform, fourier_transform_writes_data_correctly_to_grid) {
	Array3d<MyComplex> arr1(grid.gnxRealCells(),grid.gnyRealCells(),grid.gnzRealCells()), arr2(grid.gnxRealCells(),grid.gnyRealCells(),grid.gnzRealCells());
	for (int i = 0; i < arr1.gnx(); i++)
		for (int j = 0; j < arr1.gny(); j++)
			for (int k = 0; k < arr1.gnz(); k++)
				arr1(i,j,k).SetReal(grid.E.x(i, j, k));

	fftw_plan plan = fftw_plan_dft_3d(grid.gnxRealCells(), grid.gnyRealCells(), grid.gnzRealCells(), (fftw_complex*)&(arr1[0]), (fftw_complex*)&(arr2[0]), FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(plan);

	FourierTransformation(grid, E, x, RtoC);

	for (int i = 0; i < grid.gnxComplexCells(); i++)
		for (int j = 0; j < grid.gnyComplexCells(); j++)
			for (int k = 0; k < grid.gnzComplexCells(); k++) {
				ASSERT_EQ(grid.EF[0](i, j, k), arr2(i, j, k));
			}

}