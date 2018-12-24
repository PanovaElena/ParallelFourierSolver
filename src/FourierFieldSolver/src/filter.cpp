#include "filter.h"
#include "simple_types_and_constants.h"
#include "class_member_ptr.h"
#include "vec.h"

void filter1d(Grid3d& gr, int maskWidth_2, int numZeroFreq_2, Coordinate coord) {

    int a1 = gr.gnComplexCells().*GetCoord<int>(coord) / 2 - maskWidth_2 - numZeroFreq_2;
    int b1 = gr.gnComplexCells().*GetCoord<int>(coord) / 2 - numZeroFreq_2;
    int a2 = gr.gnComplexCells().*GetCoord<int>(coord) / 2 + maskWidth_2 + numZeroFreq_2;
    int b2 = gr.gnComplexCells().*GetCoord<int>(coord) / 2 + numZeroFreq_2;

    vec3<int> ind1a = vec3<int>::getVecIfCoord(coord, vec3<int>(a1), vec3<int>(0));
    vec3<int> ind1b = vec3<int>::getVecIfCoord(coord, vec3<int>(b1), gr.gnComplexCells());
    vec3<int> ind2a = vec3<int>::getVecIfCoord(coord, vec3<int>(a2), gr.gnComplexCells());
    vec3<int> ind2b = vec3<int>::getVecIfCoord(coord, vec3<int>(b2), vec3<int>(0));

    for (int i = ind1a.x; i < ind1b.x; i++)
        for (int j = ind1a.y; j < ind1b.y; j++)
            for (int k = ind1a.z; k < ind1b.z; k++) {
                gr.EF.Write(i, j, k, gr.EF(i, j, k) * MyComplex(cos((constants::pi*(i - a1)) / (b1 - a1) * 0.5)*
                    cos((constants::pi*(i - a1)) / (b1 - a1) * 0.5)));
                gr.BF.Write(i, j, k, gr.BF(i, j, k) * MyComplex(cos((constants::pi*(i - a1)) / (b1 - a1) * 0.5)*
                    cos((constants::pi*(i - a1)) / (b1 - a1) * 0.5)));
            }

    if (coord != z) {
        for (int i = ind2b.x; i < ind2a.x; i++)
            for (int j = ind2b.y; j < ind2a.y; j++)
                for (int k = ind2b.z; k < ind2a.z; k++) {
                    gr.EF.Write(i, j, k, gr.EF(i, j, k) * MyComplex(cos((constants::pi*(a2 - i)) / (a2 - b2) * 0.5)
                        * cos((constants::pi*(a2 - i)) / (a2 - b2) * 0.5)));
                    gr.BF.Write(i, j, k, gr.BF(i, j, k) * MyComplex(cos((constants::pi*(a2 - i)) / (a2 - b2) * 0.5)
                        * cos((constants::pi*(a2 - i)) / (a2 - b2) * 0.5)));
                }
    }

    for (int i = ind1b.x; i < ind2b.x; i++)
        for (int j = ind1b.y; j < ind2b.y; j++)
            for (int k = ind1b.z; k < fmin(ind2b.z, gr.gnzComplexCells()); k++) {
                gr.EF.Write(i, j, k, vec3<MyComplex>(0));
                gr.BF.Write(i, j, k, vec3<MyComplex>(0));
            }
}

void lowFrequencyFilter(Grid3d& gr, vec3<int> maskWidth, vec3<int> numZeroFreq) {
    if (maskWidth.x != 0) filter1d(gr, (maskWidth.x - numZeroFreq.x) / 2, numZeroFreq.x / 2, x);
    if (maskWidth.y != 0) filter1d(gr, (maskWidth.y - numZeroFreq.y) / 2, numZeroFreq.y / 2, y);
    if (maskWidth.z != 0) filter1d(gr, (maskWidth.z - numZeroFreq.z) / 2, numZeroFreq.z / 2, z);
}