#include "write_file.h"
#include <fstream>
#include "grid3d.h"

void WriteFileE(Grid3d& gr, std::string nameFile) {
    std::ofstream file;
    file.open(nameFile);

    for (int j = 0; j < gr.gnyRealNodes(); j++) {
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            file << gr(i, j, gr.gnzRealCells() / 2).E.getNorm() << ";";
        file << "\n";
    }

    file.close();
}