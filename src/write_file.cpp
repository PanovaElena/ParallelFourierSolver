#include "write_file.h"
#include <fstream>
#include "grid3d.h"
#include "simple_types.h"
#include "class_member_ptr.h"

void WriteFileFieldAbs2d(Field field, Grid3d& gr, std::string nameFile) {
    std::ofstream file;
    file.open(nameFile);

    for (int j = 0; j < gr.gnyRealNodes(); j++) {
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            file << (gr(i, j, gr.gnzRealCells() / 2).*GetField(field)).getNorm()<< ";";
        file << "\n";
    }

    file.close();
}

void WriteFileField2d(Field field, Coords coord, Grid3d& gr, std::string nameFile) {
    std::ofstream file;
    file.open(nameFile);

    for (int j = 0; j < gr.gnyRealNodes(); j++) {
        for (int i = 0; i < gr.gnxRealNodes(); i++)
            file << ((gr(i, j, gr.gnzRealCells() / 2).*GetField(field)).*GetCoord(coord))() << ";";
        file << "\n";
    }

    file.close();
}

void WriteFileFieldAbs1d(Field field, Grid3d & gr, std::string nameFile)
{
    std::ofstream file;
    file.open(nameFile);

    for (int i = 0; i < gr.gnxRealNodes(); i++)
        file << (gr(i, gr.gnyRealCells() / 2, gr.gnzRealCells() / 2).*GetField(field)).getNorm() << "\n";

    file.close();
}

void WriteFileField1d(Field field, Coords coord, Grid3d & gr, std::string nameFile)
{
    std::ofstream file;
    file.open(nameFile);

    for (int i = 0; i < gr.gnxRealNodes(); i++)
        file << ((gr(i, gr.gnyRealCells() / 2, gr.gnzRealCells() / 2).*GetField(field)).*GetCoord(coord))() << "\n";

    file.close();
}
