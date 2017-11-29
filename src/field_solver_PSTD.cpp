#include "field_solver.h"
#include "fftw3.h"
#include <string>
#include "grid2d.h"
#include "constants.h"
/*
int mod(int a, int b) {
	return (a + b) % b;
}

#define real 0
#define imag 1

typedef const int field;
const int NField = 9;
static const field E = 0, B = 3, I = 6;
static const field Ex = 0, Ey = 1, Ez = 2;
static const field Bx = 3, By = 4, Bz = 5;
static const field Ix = 6, Iy = 7, Iz = 8;

fftw_complex complex_i = { 0,1 };

double** CreateArr(int Nx, int Ny) {
	double ** arr = new double*[NField];
	for (int i = 0; i < NField; i++)
		arr[i] = new double[Nx*Ny];
}

void InitializeArr(Grid2d& gr, double** arr) {
	for (int i = 0; i < gr.gnx(); i++)
		for (int j = 0; j < gr.gny(); j++) {
			arr[Ex][i*gr.gnx() + j] = gr(i, j).E.x;
			arr[Ey][i*gr.gnx() + j] = gr(i, j).E.y;
			arr[Ez][i*gr.gnx() + j] = gr(i, j).E.z;
			arr[Bx][i*gr.gnx() + j] = gr(i, j).B.x;
			arr[By][i*gr.gnx() + j] = gr(i, j).B.y;
			arr[Bz][i*gr.gnx() + j] = gr(i, j).B.z;
			arr[Ix][i*gr.gnx() + j] = gr(i, j).J.x;
			arr[Iy][i*gr.gnx() + j] = gr(i, j).J.y;
			arr[Iz][i*gr.gnx() + j] = gr(i, j).J.z;
		}
}

void FourierTransformation(double** arr1, fftw_complex** arr2, int Nx, int Ny, int dir) {
	for (int i = 0; i < NField; i++) {
		fftw_plan plan;
		switch (dir) {
		case FFTW_FORWARD:
			plan = fftw_plan_dft_r2c_2d(Nx, Ny, arr1[i], arr2[i], FFTW_ESTIMATE);
			fftw_execute_dft_r2c(plan, arr1[i], arr2[i]);
			break;
		case FFTW_BACKWARD:
			plan = fftw_plan_dft_c2r_2d(Nx, Ny, arr2[i], arr1[i], FFTW_ESTIMATE);
			fftw_execute_dft_c2r(plan, arr2[i], arr1[i]);
			break;
		}
		fftw_destroy_plan(plan);
	}
}

void RefreshField(fftw_complex** arr, int Nx, int Ny, int dx, int dy, double dt) {
	for (int i = 0; i < Nx; i++)
		for (int j = 0; j < Ny; j++) {
			double omegaX = (2 * constants::pi*((i <= Nx / 2) ? i : i - Nx)) / (dx*Nx);
			double omegaY = (2 * constants::pi*((j <= Ny / 2) ? j : j - Ny)) / (dy*Ny);
			double omegaZ = (2 * constants::pi*((j <= Ny / 2) ? j : j - Ny)) / (dy*Ny);

			//arr[Ex][i*Nx+j]+= complex_i*constants::c*dt?(omegaY (B_z) ?(I, J, K) - ?_z(I, J, K) (B_y) ?(I, J, K))
			//(E_y) ?(I, J, K)=(E_y) ?(I, J, K) - ic?t?(?_x(I, J, K) (B_z) ?(I, J, K) - ?_z(I, J, K) (B_x) ?(I, J, K))
			//(E_z) ?(I, J, K)=(E_z) ?(I, J, K) + ic?t?(?_x(I, J, K) (B_y) ?(I, J, K) - ?_y(I, J, K) (B_x) ?(I, J, K))


		}
}

void FieldSolver(Grid2d & gr, double dt) {
	double** arrTmp = CreateArr(gr.gnx(), gr.gny());
	fftw_complex** arrFourier = new fftw_complex*[NField];
	for (int i = 0; i < NField; i++)
		arrFourier[i] = fftw_alloc_complex(gr.gnx()*gr.gny());
	InitializeArr(gr, arrTmp);
	FourierTransformation(arrTmp, arrFourier, gr.gnx(), gr.gny(), FFTW_FORWARD);


}
*/