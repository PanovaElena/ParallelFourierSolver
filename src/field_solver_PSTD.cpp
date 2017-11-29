#include "field_solver.h"

#include "constants.h"
/*
int mod(int a, int b) {
	return (a + b) % b;
}

#define real 0
#define imag 1


void RefreshField(fftw_complex** arr, int Nx, int Ny, int Nz, double dx, double dy, double dz, double dt) {
	for (int i = 0; i < Nx; i++)
		for (int j = 0; j < Ny; j++) 
			for (int k = 0; k < Nz; k++) {
				double omegaX = (2 * constants::pi*((i <= Nx / 2) ? i : i - Nx)) / (dx*Nx);
				double omegaY = (2 * constants::pi*((j <= Ny / 2) ? j : j - Ny)) / (dy*Ny);
				double omegaZ = (2 * constants::pi*((k <= Nz / 2) ? k : k - Nz)) / (dz*Nz);

				//arr[Ex][i*Nx+j]+= complex_i*constants::c*dt?(omegaY (B_z) ?(I, J, K) - ?_z(I, J, K) (B_y) ?(I, J, K))
				//(E_y) ?(I, J, K)=(E_y) ?(I, J, K) - ic?t?(?_x(I, J, K) (B_z) ?(I, J, K) - ?_z(I, J, K) (B_x) ?(I, J, K))
				//(E_z) ?(I, J, K)=(E_z) ?(I, J, K) + ic?t?(?_x(I, J, K) (B_y) ?(I, J, K) - ?_y(I, J, K) (B_x) ?(I, J, K))


			}
}

void FieldSolver(Grid3d & gr, double dt) {
	double** arrTmp = CreateArr(gr.gnx(), gr.gny(), gr.gnz());
	fftw_complex** arrFourier = new fftw_complex*[NField];
	for (int i = 0; i < NField; i++)
		arrFourier[i] = fftw_alloc_complex(gr.gnx()*gr.gny()*gr.gnz());
	InitializeArr(gr, arrTmp);
	FourierTransformation(arrTmp, arrFourier, gr.gnx(), gr.gny(), gr.gnz(), FFTW_FORWARD);


}
*/