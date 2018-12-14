#include "filter.h"

void lowFrequencyFilter(Grid3d& gr, int maskWidth, int numZeroFreq) {

    double a1 = gr.gnComplexCells().x / 2 - maskWidth - numZeroFreq;
    double b1 = gr.gnComplexCells().x / 2 - numZeroFreq;
    double a2 = gr.gnComplexCells().x / 2 + maskWidth + numZeroFreq;
    double b2 = gr.gnComplexCells().x / 2 + numZeroFreq;

    for (int i = a1; i <= b1; i++)
        for (int j = 0; j < gr.gnComplexCells().y; j++)
            for (int k = 0; k < gr.gnComplexCells().z; k++) {
                gr.EF.Write(i, j, k, gr.EF(i, j, k) * MyComplex(cos((constants::pi*(i - a1)) / (b1 - a1) * 0.5)*
                    cos((constants::pi*(i - a1)) / (b1 - a1) * 0.5)));
                gr.BF.Write(i, j, k, gr.BF(i, j, k) * MyComplex(cos((constants::pi*(i - a1)) / (b1 - a1) * 0.5)*
                    cos((constants::pi*(i - a1)) / (b1 - a1) * 0.5)));
            }

    for (int i = b2; i <= a2; i++)
        for (int j = 0; j < gr.gnComplexCells().y; j++)
            for (int k = 0; k < gr.gnComplexCells().z; k++) {
                gr.EF.Write(i, j, k, gr.EF(i, j, k) * MyComplex(cos((constants::pi*(a2 - i)) / (a2 - b2) * 0.5)
                    * cos((constants::pi*(a2 - i)) / (a2 - b2) * 0.5)));
                gr.BF.Write(i, j, k, gr.BF(i, j, k) * MyComplex(cos((constants::pi*(a2 - i)) / (a2 - b2) * 0.5)
                    * cos((constants::pi*(a2 - i)) / (a2 - b2) * 0.5)));
            }

    for (int i = b1 + 1; i <= b2 - 1; i++)
        for (int j = 0; j < gr.gnComplexCells().y; j++)
            for (int k = 0; k < gr.gnComplexCells().z; k++) {
                gr.EF.Write(i, j, k, vec3<MyComplex>(0));
                gr.BF.Write(i, j, k, vec3<MyComplex>(0));
            }
}