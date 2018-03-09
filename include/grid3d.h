#pragma once
#include "vec.h"
#include "my_complex.h"
#include "iostream"
#include "array3d.h"

struct Node
{
	vec3<double> B;
	vec3<double> E;

	vec3<MyComplex> BF;
	vec3<MyComplex> EF;

	Node() :B(0,0,0), E(0, 0, 0) {}
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
	int nx; //храним число ячеек
	int ny;
	int nz;

	double ax; //абсцисса начала рабочей области
	double bx; //абсцисса конца рабочей области
	double ay; //ордината начала рабочей области
	double by; //ордината конца рабочей области
	double az; //аппликата начала рабочей области
	double bz; //аппликата конца рабочей области

	double dx;// dx = x / nx
	double dy;// dy = y / ny
	double dz;// dz = z / nz

	Array3d<Node> nodes;

	void clearGrid();
public:
	Grid3d();
	Grid3d(const Grid3d& gr);
	Grid3d(int _nx, int _ny, int _nz, double _ax, double _bx, double _ay, double _by, double _az, double _bz);
	~Grid3d();
	//сравнение только по вещественным полям
	int operator==(const Grid3d& grid2);
	//только сетки одинаковой размерности, присвоение поля
	Grid3d& operator=(const Grid3d& grid2);

	void Initialize(int _nx, int _ny, int _nz, double _ax, double _bx, double _ay, double _by, double _az, double _bz);

	int gnxRealCells() const;//get nx 
	int gnyRealCells() const;//get ny
	int gnzRealCells() const;//get nz

	int gnxComplexCells() const;//get nx 
	int gnyComplexCells() const;//get ny
	int gnzComplexCells() const;//get nz/2+1

	int gnxRealNodes() const;//get nx+1
	int gnyRealNodes() const;//get ny+1
	int gnzRealNodes() const;//get nz+1

	double gdx() const; //get dx
	double gdy() const;//get dy
	double gdz() const;//get dz

	double gax() const; //get ax
	double gay() const;//get ay
	double gaz() const;//get az

	double gbx() const; //get bx
	double gby() const;//get by
	double gbz() const;//get bz

	Node& operator()(int i, int j, int k) { 
		if (i > nx || j > ny || k>nz) 
		{
			throw "bad index";
		}
		return nodes(i,j,k); 
	}

	Node& operator()(vec3<int> ind) {
		if (ind[0] > nx || ind[1] > ny || ind[2]>nz)
		{
			throw "bad index";
		}
		return nodes(ind[0],ind[1],ind[2]);
	}


};