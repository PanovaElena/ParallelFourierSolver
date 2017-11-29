#pragma once
#include "vec.h"

struct node
{
	vec3 B;
	vec3 E;
	vec3 J;

	node() :B(), E(), J() {}
	node(const node& n) :B(n.B), E(n.E), J(n.J) {}
};
class Grid2d
{
private:
	int nx; //храним число ячеек
	int ny;

	double ax; //абсцисса начала рабочей области
	double bx; //абсцисса конца рабочей области
	double ay; //ордината начала рабочей области
	double by; //ордината конца рабочей области

	double dx;// dx = x / nx
	double dy;// dy = y / ny

	node **nodes;
	void clearGrid();
public:
	Grid2d();
	Grid2d(int _nx, int _ny, double _x, double _y);
	Grid2d(int _nx, int _ny, double _x, double _y, vec3 B, vec3 E);
	Grid2d(int _nx, int _ny, double _ax, double _ay, double _bx, double _by);
	Grid2d(int _nx, int _ny, double _ax, double _ay, double _bx, double _by, vec3 B, vec3 E);
	Grid2d(const Grid2d &gr);
	~Grid2d();

	void Initialize(int _nx, int _ny, double _ax, double _ay, double _bx, double _by);

	Grid2d& operator = (const Grid2d &gr);

	node &getRefNode(int i, int j);//getter + setter, remove

	vec4 getEx(vec2i v) const; //Возвращает значение электрического поля (по оси OX) для 4 узлов 
	vec4 getEy(vec2i v) const; //Возвращает значение электрического поля (по оси OY) для 4 узлов 
	vec4 getEz(vec2i v) const; //Возвращает значение электрического поля (по оси OZ) для 4 узлов 
	vec4 getBx(vec2i v) const;
	vec4 getBy(vec2i v) const;
	vec4 getBz(vec2i v) const;

	int gnx() const;//get nx 
	int gny() const;//get ny

	double gdx() const; //get dx
	double gdy() const;//get dy

	double gax() const; //get ax
	double gay() const;//get ay

	double gbx() const; //get bx
	double gby() const;//get by

	bool inside(vec3 v) const;

	vec2i getIndex(double x, double y) const;
	vec2 getLocalCoord(double x, double y) const;
	node& operator()(int i, int j) { 
		if (i > nx || j > ny) 
		{

			throw "bad index";
		}
		return nodes[i][j]; 
	}

};