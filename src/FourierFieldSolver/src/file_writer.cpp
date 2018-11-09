#include <fstream>
#include "grid3d.h"
#include "simple_types_and_constants.h"
#include "class_member_ptr.h"
#include <iomanip>

void SetSymb(Section::Plane plane, std::string& si, std::string& sj, std::string& sk) {
    switch (plane) {
    case Section::XOY:
        si = ";"; sj = "\n"; sk = "";
        break;
    case Section::XOZ:
        si = ";"; sj = ""; sk = "\n";
        break;
    case Section::YOZ:
        si = ""; sj = ";"; sk = "\n";
        break;
    default:
        break;
    }
}

void FileWriter::Write(Grid3d & gr, std::string name, std::string si, std::string sj, std::string sk) {
    std::ofstream file(dir + name);

    for (int k = section.startZ; k <= section.endZ; k++) {
        for (int j = section.startY; j <= section.endY; j++) {
            for (int i = section.startX; i <= section.endX; i++)
                file << std::setprecision(15)<<(gr(i, j, k).*GetField(field).*GetCoord(coord))() << si;
            file << sj;
        }
        file << sk;
    }

    file.close();
}

void FileWriter::WriteFile0d(Grid3d & gr, std::string name)
{
    Write(gr, name, "\n", "", "");
}

void FileWriter::WriteFile1d(Grid3d & gr, std::string name)
{
    Write(gr, name, "\n", "", "");
}

void FileWriter::WriteFile2d(Grid3d & gr, std::string name)
{
    std::string si, sj, sk;
    SetSymb(section.plane1, si, sj, sk);
    Write(gr, name, si, sj, sk);
} 

void SetCoordUsingLocation(Section::LocationOfPlane loc, int& start, int& end, int n) {
    switch (loc) {
    case Section::start:
        start = end = 0;
        break;
    case Section::center:
        start = end = n/2;
        break;
    case Section::end:
        start = end = n;
        break;
    default:
        break;
    }
}

void Section::SetBorders(Grid3d& gr)
{
    startX = startY = startZ = 0;
    endX = gr.gnxRealCells();
    endY = gr.gnyRealCells();
    endZ = gr.gnzRealCells();
    if (plane1 == Plane::XOY || plane2 == Plane::XOY || plane3 == Plane::XOY) {
        LocationOfPlane loc = plane1 == Plane::XOY ? loc1 : plane2 == Plane::XOY ? loc2 : plane3 == Plane::XOY? loc3 : LocationOfPlane::noneLocation;
        SetCoordUsingLocation(loc, startZ, endZ, gr.gnzRealCells());
    }
    if (plane1 == Plane::XOZ || plane2 == Plane::XOZ || plane3 == Plane::XOZ) {
        LocationOfPlane loc = plane1 == Plane::XOZ ? loc1 : plane2 == Plane::XOZ ? loc2 : plane3 == Plane::XOZ ? loc3 : LocationOfPlane::noneLocation;
        SetCoordUsingLocation(loc, startY, endY, gr.gnyRealCells());
    }
    if (plane1 == Plane::YOZ || plane2 == Plane::YOZ || plane3 == Plane::YOZ) {
        LocationOfPlane loc = plane1 == Plane::YOZ ? loc1 : plane2 == Plane::YOZ ? loc2 : plane3 == Plane::YOZ ? loc3 : LocationOfPlane::noneLocation;
        SetCoordUsingLocation(loc, startX, endX, gr.gnxRealCells());
    }
}
