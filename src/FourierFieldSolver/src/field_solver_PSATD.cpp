#include "field_solver.h"
#include "grid3d.h"
#include "physical_constants.h"
#include "fourier_transformation.h"
#include "my_complex.h"

void FieldSolverPSATD(Grid3d & gr, double dt) {
    for (int i = 0; i < gr.gnxComplexCells(); i++)
        for (int j = 0; j < gr.gnyComplexCells(); j++)
            for (int k = 0; k < gr.gnzComplexCells(); k++) {

                vec3<MyComplex> K(OmegaX(i, gr), OmegaY(j, gr), OmegaZ(k, gr));

                double normK = K.getNorm();
                K = K.Normalize();
                double C = cos(normK*constants::c*dt);
                double S = sin(normK*constants::c*dt);
                vec3<MyComplex> E = gr(i, j, k).EF, B = gr(i, j, k).BF,
                    J = (4 * constants::pi)*gr(i, j, k).JF;
                vec3<MyComplex> Jl = K*vec3<MyComplex>::ScalarProduct(K, J);
                vec3<MyComplex> El = K*vec3<MyComplex>::ScalarProduct(K, E);

                if (K == vec3<MyComplex>(0))
                {
                    gr(i, j, k).EF = (-1)*J;
                    gr(i, j, k).BF = vec3<MyComplex>(0);
                    continue;
                }

                gr(i, j, k).EF = C*E + complex_i*S*vec3<MyComplex>::VectorProduct(K, B) - 
                    S / (normK*constants::c)*J + (1 - C)*El + Jl*(S / (normK*constants::c) - dt);
                gr(i, j, k).BF = C*B - complex_i*S*vec3<MyComplex>::VectorProduct(K, E) +
                    complex_i*((1 - C) / (normK*constants::c))*vec3<MyComplex>::VectorProduct(K, J);
            }
}