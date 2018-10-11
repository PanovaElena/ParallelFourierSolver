#include "gtest.h"
#include "fourier_transformation.h"
#include "grid3d.h"
#include "my_complex.h"
#include "physical_constants.h"

class test_lag : public testing::Test {
public:

	int n = 6;
	double a = 0, b = n;
	Grid3d gr;
	double A = 1000;
	double d = (b - a) / n;
	int step = 1;
	double c = step * d;

	test_lag() : gr(n, n, n, a, b, a, b, a, b) {
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				for (int k = 0; k < n; k++)
					gr(i, j, k).E[0] = f(i, j, k);
	}

	double f(int i, int j, int k) {
		return A*sin(2*constants::pi*(i*gr.gdx()+ j*gr.gdy()+ k*gr.gdz())/(b-a));
	}

	void MyTestBodyLag() {
		FourierTransformation(gr, RtoC);
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				for (int k = 0; k < n / 2 + 1; k++)
				{
					double omegaX = OmegaX(i, gr);
					double omegaY = OmegaY(j, gr);
					double omegaZ = OmegaZ(k, gr);

					gr(i, j, k).EF[0] *= MyComplex::GetTrig(1, -c * (omegaX + omegaY + omegaZ));
				}
		FourierTransformation(gr, CtoR);
	}

};

TEST_F(test_lag, lag_is_correct) {
	
	MyTestBodyLag();

	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			for (int k = 0; k < n; k++) {
				ASSERT_NEAR(f(i - step, j - step, k - step), gr(i, j, k).E.get_x(), 1E-7);
			}
}

TEST_F(test_lag, last_nodes_are_correct) {

	MyTestBodyLag();

	for (int i = 0; i <= n; i++)
		for (int j = 0; j <= n; j++)
			for (int k = 0; k <= n; k++)
                if (i%n == 0 && j % n == 0 || i%n == 0 && k % n == 0 || j%n == 0 && k % n == 0)
                {
                    ASSERT_DOUBLE_EQ(gr(i%n, j%n, k%n).E.get_x(), gr(i, j, k).E.get_x());
                }
}
