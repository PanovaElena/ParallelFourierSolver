#include "field_solver.h"
#include "grid3d.h"
#include "physical_constants.h"
#include "fourier_transformation.h"
#include "my_complex.h"

void fieldSolverPSATD(Grid3d & gr, double dt) {
#pragma omp parallel for
    for (int i = 0; i < gr.sizeComplex().x; i++)
        for (int j = 0; j < gr.sizeComplex().y; j++)
            for (int k = 0; k < gr.sizeComplex().z; k++) {
                vec3<MyComplex> K = getFreqVector(vec3<int>(i, j, k), gr);
                double normK = K.getNorm();
                K = K.normilize();
                double C = cos(normK*constants::c*dt);
                double S = sin(normK*constants::c*dt);
                vec3<MyComplex> E = gr.EF(i, j, k), B = gr.BF(i, j, k),
                    J = 4 * constants::pi * gr.JF(i, j, k);
                vec3<MyComplex> Jl = K * vec3<MyComplex>::dot(K, J);
                vec3<MyComplex> El = K * vec3<MyComplex>::dot(K, E);

                if (normK == 0) {
                    gr.EF.write(i, j, k, (-1)*J);
                    gr.BF.write(i, j, k, vec3<MyComplex>(0));
                    continue;
                }

                gr.EF.write(i, j, k, C*E + complex_i * S*vec3<MyComplex>::cross(K, B) -
                    S / (normK*constants::c)*J + (1 - C)*El + Jl * (S / (normK*constants::c) - dt));
                gr.BF.write(i, j, k, C*B - complex_i * S*vec3<MyComplex>::cross(K, E) +
                    complex_i * ((1 - C) / (normK*constants::c))*vec3<MyComplex>::cross(K, J));
            }

}