#pragma once
#include "vec.h"
#include "my_complex.h"
#include "iostream"

struct node
{
	vec3<double> B;
	vec3<double> E;
	//vec3 J;

	vec3<MyComplex> BF;
	vec3<MyComplex> EF;

	node() :B(0,0,0), E(0, 0, 0) {}
	node(const node& n) :B(n.B), E(n.E) {}
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

	node ***nodes;

	node* tmp1;//структура выделения памяти
	node** tmp2;

	void clearGrid();
	node& operator()(int i) const {
		return  tmp1[i];
	}
public:
	Grid3d();
	Grid3d(const Grid3d& gr);
	Grid3d(int _nx, int _ny, int _nz, double _ax, double _bx, double _ay, double _by, double _az, double _bz);
	~Grid3d();
	//сравнение только по вещественным полям
	int operator==(const Grid3d& grid2);
	//только сетки одинаковой размерности, присвоение поля
	void operator=(const Grid3d& grid2);

	void Initialize(int _nx, int _ny, int _nz, double _ax, double _bx, double _ay, double _by, double _az, double _bz);

	int gnxCells() const;//get nx 
	int gnyCells() const;//get ny
	int gnzCells() const;//get nz

	int gnxNodes() const;//get nx+1
	int gnyNodes() const;//get ny+1
	int gnzNodes() const;//get nz+1

	double gdx() const; //get dx
	double gdy() const;//get dy
	double gdz() const;//get dz

	double gax() const; //get ax
	double gay() const;//get ay
	double gaz() const;//get az

	double gbx() const; //get bx
	double gby() const;//get by
	double gbz() const;//get bz

	node& operator()(int i, int j, int k) { 
		if (i > nx || j > ny || k>nz) 
		{
			throw "bad index";
		}
		return nodes[i][j][k]; 
	}

};