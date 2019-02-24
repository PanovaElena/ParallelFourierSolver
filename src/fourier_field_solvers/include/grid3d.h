#pragma once
#include "vec.h"
#include "my_complex.h"
#include "iostream"
#include "array3d.h"
#include "simple_types_and_constants.h"

template <class T>
struct FieldForGrid: public vec3<Array3d<T>> {

    vec3<T> operator()(int i, int j, int k)
    {
        return vec3<T>(this->x(i, j, k), this->y(i, j, k), this->z(i, j, k));
    }
    vec3<T> operator()(vec3<int> v)
    {
        return (*this)(v.x, v.y, v.z);
    }
    void Write(int i, int j, int k, vec3<T> val) {
        this->x(i, j, k) = val.x;
        this->y(i, j, k) = val.y;
        this->z(i, j, k) = val.z;
    }
    void Clear() {
        this->x.Clear();
        this->y.Clear();
        this->z.Clear();
    }
    void Initialize(int nx, int ny, int nz) {
        this->x.Initialize(nx, ny, nz);
        this->y.Initialize(nx, ny, nz);
        this->z.Initialize(nx, ny, nz);
    }
    friend bool operator==(const FieldForGrid& f1, const FieldForGrid& f2) {
        return (f1.x == f2.x && f1.y == f2.y && f1.z == f2.z);
    }
    friend bool operator!=(const FieldForGrid& f1, const FieldForGrid& f2) {
        return !(f1 == f2);
    }
};

class Grid3d
{
private:
    vec3<int> n;

    vec3<double>a; // начало рабочей области
    vec3<double>b; // конец рабочей области

    vec3<double> d; // dx = x / nx, dy = y / ny, dz = z / nz

    void clearGrid();

    Type state = Double; // fourier grid or real

public:
    FieldForGrid<double> E;
    FieldForGrid<double> B;
    FieldForGrid<double> J;
    FieldForGrid<MyComplex> EF;
    FieldForGrid<MyComplex> BF;
    FieldForGrid<MyComplex> JF;

public:
    Grid3d();
    Grid3d(const Grid3d& gr);
    Grid3d(vec3<int> n, vec3<double> a, vec3<double> b);
    Grid3d(int _nx, int _ny, int _nz, double _ax, double _bx, double _ay, double _by, double _az, double _bz);
    ~Grid3d();
    //сравнение только по вещественным полям
    int operator==(const Grid3d& grid2);

    Grid3d& operator=(const Grid3d& grid2);

    void Initialize(vec3<int> n, vec3<double> _a, vec3<double> b);

    int gnxRealCells() const;//get nx 
    int gnyRealCells() const;//get ny
    int gnzRealCells() const;//get nz
    vec3<int> gnRealCells() const;

    int gnxComplexCells() const;//get nx 
    int gnyComplexCells() const;//get ny
    int gnzComplexCells() const;//get nz/2+1
    vec3<int> gnComplexCells() const;

    int gnxRealNodes() const;//get nx
    int gnyRealNodes() const;//get ny
    int gnzRealNodes() const;//get nz
    vec3<int> gnRealNodes() const;

    double gdx() const; //get dx
    double gdy() const;//get dy
    double gdz() const;//get dz
    vec3<double> gd() const;

    double gax() const; //get ax
    double gay() const;//get ay
    double gaz() const;//get az
    vec3<double> ga() const;

    double gbx() const; //get bx
    double gby() const; //get by
    double gbz() const; //get bz
    vec3<double> gb() const;

    Type getState() {
        return state;
    }
    Direction getLastFourierTransform() {
        switch (state) {
        case Complex:
            return RtoC;
        default:
            return CtoR;
        }
    }

    void setLastFourierTransform(Direction dir) {
        switch (dir) {
        case RtoC:
            state = Complex;
            break;
        default:
            state = Double;
            break;
        }
    }
    void setState(Type _state) {
        state = _state;
    }

};