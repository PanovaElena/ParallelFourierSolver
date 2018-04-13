#include "grid3d.h"
#include "constants.h"
#include "fourier_transformation.h"
#include "my_complex.h"
#include "field_solver.h"
#include "mpi.h"

Grid3d GetDomain(Grid3d & gr, double borderWidthPerc/*ширина области пересечения подсеток в процентах oт размера подсетки*/) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int width = gr.gnxRealCells() / size;
    int borderWidth = (int)(width*borderWidthPerc);
    int n1 = width*rank, n2 = width*(rank + 1);

    Grid3d gr2;
    double a, b;
    int n;
    CalcABN(gr, a, b, n, rank, size, n1, n2, borderWidth);

    gr2 = Grid3d(a, b, n, gr.gay(), gr.gby(), gr.gnyRealCells(), gr.gaz(), gr.gbz(), gr.gnzRealCells());
    FillGr(gr, gr2, a, b);

}

void CalcABN(Grid3d& gr, double& a, double& b, int& n, int rank, int size, int n1, int n2, int borderWidth) {
    if (rank > 0 && rank < size - 1) {
        a = (n1 - borderWidth)*gr.gdx();
        b = (n2 + borderWidth)*gr.gdx();
        n = n2 - n1 + 2 * borderWidth;
    }
    else if (rank == 1) {
        a = n1*gr.gdx();
        b = (n2 + borderWidth)*gr.gdx();
        n = n2 - n1 + borderWidth;
    }
    else if (rank == size - 1) {
        a = (n1 - borderWidth)*gr.gdx();
        b = n2*gr.gdx();
        n = n2 - n1 + borderWidth;
    }
}

void FillGr(Grid3d& gr, Grid3d& gr2, double a, double b) {
    
}