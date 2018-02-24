#include "gtest.h"
#include "fourier_transformation.h"
#include "grid3d.h"
#include "my_complex.h"
#include "constants.h"

class test_lag : public testing::Test {
public:
	int n = 20;
	double a = 0, b = 10;
	Grid3d gr;

	test_lag() : gr(n, n, n, a, b, a, b, a, b) {
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				for (int k = 0; k < n; k++)
					gr(i, j, k).E[0] = sin(i*gr.gdx() + j*gr.gdy() + k*gr.gdz());
	}

	double f(int i, int j, int k) {
		return sin(i*gr.gdx() + j*gr.gdy() + k*gr.gdz());
	}

};

TEST_F(test_lag, lag_is_correct) {
	int step = 1;
	double c = step*gr.gdx();

	FourierTransformation(gr, RtoC);
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			for (int k = 0; k < n/2+1; k++) {
				double omegaX = (2 * constants::pi*((i <= gr.gnxCells() / 2) ? i : i - gr.gnxCells())) / (gr.gbx() - gr.gax());
				double omegaY = (2 * constants::pi*((j <= gr.gnyCells() / 2) ? j : j - gr.gnyCells())) / (gr.gby() - gr.gay());
				double omegaZ = (2 * constants::pi*((k <= gr.gnzCells() / 2) ? k : k - gr.gnzCells())) / (gr.gbz() - gr.gaz());

				gr(i, j, k).EF[0] *= MyComplex(cos(-c*(omegaX + omegaY + omegaZ)) ,sin(-c*(omegaX + omegaY + omegaZ)));
			}
	FourierTransformation(gr, CtoR);

	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			for (int k = 0; k < n; k++) {
				ASSERT_NEAR(f(i - step, j - step, k - step), gr(i, j, k).E.x(), 10 * E - 5);
			}


}