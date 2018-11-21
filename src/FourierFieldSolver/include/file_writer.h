#pragma once
class Grid3d;
#include "simple_types_and_constants.h"
#include <string>
#include "iostream"

class Section {
public:
    enum Plane {
        nonePlane,
        XOY,
        XOZ,
        YOZ
    };

    enum LocationOfPlane {
        noneLocation,
        start,
        center,
        end
    };

private:
    int startX, startY, startZ, endX, endY, endZ;

    Dimension dim = Dimension::d0;
    Plane plane1, plane2, plane3;
    LocationOfPlane loc1, loc2, loc3;

    void SetBorders(Grid3d& gr);

public:
    Section() {}
    Section(Plane _plane1, LocationOfPlane _loc1,
        Plane _plane2 = Plane::nonePlane, LocationOfPlane _loc2 = LocationOfPlane::noneLocation,
        Plane _plane3 = Plane::nonePlane, LocationOfPlane _loc3 = LocationOfPlane::noneLocation) :
        plane1(_plane1), loc1(_loc1), plane2(_plane2), loc2(_loc2), plane3(_plane3), loc3(_loc3) {
        if (plane1 == Plane::nonePlane || loc1 == LocationOfPlane::noneLocation) {
            plane1 = plane2 = plane3 = Plane::nonePlane;
            loc1 = loc2 = loc3 = LocationOfPlane::noneLocation;
        }
        SetDim();
    }

    void SetDim() {
        int idim = 3;
        if (plane1 != Plane::nonePlane && loc1 != LocationOfPlane::noneLocation) idim--;
        if (plane2 != Plane::nonePlane && loc2 != LocationOfPlane::noneLocation) idim--;
        if (plane3 != Plane::nonePlane && loc3 != LocationOfPlane::noneLocation) idim--;
        dim = (Dimension)idim;
    }

    friend class FileWriter;
};

class FileWriter {
private:
    std::string dir = "";
    Field field;
    Coordinate coord;
    Section section;

public:
    FileWriter() {}
    FileWriter(std::string _dir, Field _field, Coordinate _coord, Section _section) {
        dir = _dir;
        field = _field;
        coord = _coord;
        section = _section;
    }
    FileWriter(const FileWriter& fileWriter, std::string _dir) {
        *this = fileWriter;
        dir = _dir;
    }
    void Initialize(std::string _dir, Field _field, Coordinate _coord, const Section& _section) {
        dir = _dir;
        field = _field;
        coord = _coord;
        section = _section;
    }
    void SetSection(const Section& s) {
        section = s;
    }

    void WriteFile(Grid3d& gr, std::string name, Type t = Type::Double, std::string message="") {
        if (message!="") std::cout << message << "\n";
        section.SetBorders(gr);
        switch (section.dim) {
        case d0:
            WriteFile0d(gr, name, t);
            break;
        case d1: 
            WriteFile1d(gr, name, t);
            break;
        case d2: 
            WriteFile2d(gr, name, t);
            break;
        default: 
            std::cout << "ERROR!!!!! Cannot write to file array with dimension 3 or more\n";
            break;
        }
    }

    Coordinate getCoord() { return coord; }
    Field getField() { return field; }
    std::string getDir() { return dir; }
    Section getSection() { return section; }

    void ChangeDir(std::string newDir) {
        dir = newDir;
    }

protected:
    void WriteFile0d(Grid3d& gr, std::string name, Type type);
    void WriteFile1d(Grid3d& gr, std::string name, Type type);
    void WriteFile2d(Grid3d& gr, std::string name, Type type);
    void Write(Grid3d & gr, std::string name, Type type, std::string si, std::string sj, std::string sk);
};