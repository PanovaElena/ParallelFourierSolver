#include "grid3d.h"
#include "constants.h"
#include "fourier_transformation.h"
#include "my_complex.h"
#include "field_solver.h"
#include "mpi.h"
#include <cmath>

void CalcABN(Grid3d& gr, double& a, double& b, int& n, int rank, int size, int n1, int n2, int guardWidth) {
    if (rank > 0 && rank < size - 1) {
        a = (n1 - guardWidth)*gr.gdx();
        b = (n2 + guardWidth)*gr.gdx();
        n = n2 - n1 + 2 * guardWidth;
    }
    else if (rank == 1) {
        a = n1*gr.gdx();
        b = (n2 + guardWidth)*gr.gdx();
        n = n2 - n1 + guardWidth;
    }
    else if (rank == size - 1) {
        a = (n1 - guardWidth)*gr.gdx();
        b = n2*gr.gdx();
        n = n2 - n1 + guardWidth;
    }
}

Grid3d CreateGrid(Grid3d& gr, int rank, int size, int n1, int n2, int guardWidth) {
    Grid3d gr2;
    double a, b;
    int n;
    CalcABN(gr, a, b, n, rank, size, n1, n2, guardWidth);
    gr2 = Grid3d(n, gr.gnyRealCells(), gr.gnzRealCells(), a, b, gr.gay(), gr.gby(), gr.gaz(), gr.gbz());
    return gr;
}

void FillGrid(Grid3d& gr, Grid3d& gr2, int n0, int n1) {
    for (int i = n0; i <= n1; i++)
        for (int j = 0; j <= gr.gnyRealCells(); j++)
            for (int k = 0; j <= gr.gnzRealCells(); k++)
                gr2(i - n0, j, k) = gr(i, j, k);
}

Grid3d GetDomain(Grid3d & gr, double guardWidthPerc/*ширина области пересечения подсеток в процентах oт размера подсетки*/) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int width = gr.gnxRealCells() / size;
    int guardWidth = (int)(width*guardWidthPerc);
    int n1 = width*rank, n2 = width*(rank + 1);

    //создаем сетку с учетом guard областей
    Grid3d gr2=CreateGrid(gr, rank, size, n1, n2, guardWidth);

    //заполняем сетку без учета guard областей
    FillGrid(gr, gr2, n1, n2);

    return gr2;
}

void SetToZerosGuard(Grid3d& gr, int guardWidth) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank != 0)
        for (int i = 0; i < guardWidth; i++)
            for (int j = 0; j <= gr.gnyRealCells(); j++)
                for (int k = 0; j <= gr.gnzRealCells(); k++)
                    gr(i, j, k) = Node();
    if (rank != size - 1)
        for (int i = gr.gnxRealCells() - guardWidth + 1; i <= gr.gnxRealCells(); i++)
            for (int j = 0; j <= gr.gnyRealCells(); j++)
                for (int k = 0; j <= gr.gnzRealCells(); k++)
                    gr(i, j, k) = Node();
}

//упаковывает вещественные поля части сетки
void PackQuardData(Grid3d& gr, int n1, int n2, double*& arr, int& size) {
    const int n = 2, d = 3;//2 вектора (B, E) из 3 компонент
    size = n*d*(n2 - n1 + 1)*gr.gnyRealNodes()*gr.gnzRealNodes();
    arr = new double[size];
    for (int i = n1; i < n2; i++)
        for (int j = 0; j < gr.gnyRealNodes(); j++)
            for (int k = 0; k < gr.gnzRealNodes(); k++)
                for (int coord = 0; i < 2; coord++) {
                    arr[n*d*(i*gr.gnyRealNodes()*gr.gnzRealNodes() + j*gr.gnzRealNodes() + k) + n*coord + 0] = gr(i, j, k).E[coord];
                    arr[n*d*(i*gr.gnyRealNodes()*gr.gnzRealNodes() + j*gr.gnzRealNodes() + k) + n*coord + 1] = gr(i, j, k).B[coord];
                }
}

void UnPackQuardData(Grid3d& gr, int n1, int n2, double* arr) {
    const int n = 2, d = 3;//2 вектора (B, E) из 3 компонент
    for (int i = n1; i < n2; i++)
        for (int j = 0; j < gr.gnyRealNodes(); j++)
            for (int k = 0; k < gr.gnzRealNodes(); k++)
                for (int coord = 0; i < 2; coord++) {
                    gr(i, j, k).E[coord] = arr[n*d*(i*gr.gnyRealNodes()*gr.gnzRealNodes() + j*gr.gnzRealNodes() + k) + n*coord + 0];
                    gr(i, j, k).B[coord] = arr[n*d*(i*gr.gnyRealNodes()*gr.gnzRealNodes() + j*gr.gnzRealNodes() + k) + n*coord + 1];
                }
}

