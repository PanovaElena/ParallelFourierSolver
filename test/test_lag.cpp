#include "gtest.h"
#include "fourier_transformation.h"
#include "grid3d.h"
#include "my_complex.h"
#include "constants.h"

class test_lag : public testing::Test {
public:
	int n = 6;
	double a = 0, b = n;
	Grid3d gr;
	double A = 1000;
	double d = (b - a) / n;

	test_lag() : gr(n, n, n, a, b, a, b, a, b) {
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				for (int k = 0; k < n; k++)
					gr(i, j, k).E[0] = f(i,j,k);
	}

	double f(int i, int j, int k) {
		return sin(A*2*constants::pi*(i*gr.gdx()+ j*gr.gdy()+ k*gr.gdz())/(b-a));
	}

};

TEST_F(test_lag, lag_is_correct) {
	int step = 1;
	double c = step*d;

	FourierTransformation(gr, RtoC);
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			for (int k = 0; k < n/2+1; k++)
			{
				double omegaX = OmegaX(i, gr);
				double omegaY = OmegaY(j, gr);
				double omegaZ = OmegaZ(k, gr);

				gr(i, j, k).EF[0] *= MyComplex::GetTrig(1, -c*(omegaX + omegaY + omegaZ));
			}
	FourierTransformation(gr, CtoR);

	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			for (int k = 0; k < n; k++) {
				ASSERT_NEAR(f(i - step, j - step, k - step), gr(i, j, k).E.x(), 1E-7);
			}
}
