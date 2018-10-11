#pragma once
#include "vec.h"
#include "my_complex.h"
#include "iostream"
#include "array3d.h"

struct Node
{
    vec3<double> B;
    vec3<double> E;
    vec3<double> J;

    vec3<MyComplex> BF;
    vec3<MyComplex> EF;
    vec3<MyComplex> JF;

    Node() :B(0,0,0), E(0, 0, 0), J(0,0,0) {}
    Node(const Node& n) :B(n.B), E(n.E) {}
    //только вещественные поля
    friend int operator==(const Node& n1, const Node& n2) {
        return (n1.B == n2.B && n1.E == n2.E);
    }
    friend int operator!=(const Node& n1, const Node& n2) {
        return !(n1==n2);
    }
};

class Grid3d
{
private:
    vec3<int> n;

    vec3<double>a; // начало рабочей области
    vec3<double>b; // конец рабочей области

    vec3<double> d; // dx = x / nx, dy = y / ny, dz = z / nz

    Array3d<Node> nodes;

    void clearGrid();
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

	int gnxRealNodes() const;//get nx+1
	int gnyRealNodes() const;//get ny+1
	int gnzRealNodes() const;//get nz+1
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

	Node& operator()(int i, int j, int k) { 
		if (i > n.get_x() || j > n.get_y() || k>n.get_z()) 
		{
			throw "wrong index";
		}
		return nodes(i,j,k); 
	}

	Node& operator()(vec3<int> ind) {
		if (ind.x > n.get_x() || ind.y > n.get_y() || ind.z>n.get_z())
		{
			throw "wrong index";
		}
		return nodes(ind.x,ind.y,ind.z);
	}


};