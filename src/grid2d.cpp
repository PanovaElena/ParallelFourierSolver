#include "grid2d.h"

Grid2d::Grid2d():nodes(0){}

Grid2d::Grid2d(int _nx, int _ny, double _x, double _y) 
{
	Initialize(_nx, _ny, 0, 0, _x, _y);
}
Grid2d::Grid2d(int _nx, int _ny, double _x, double _y, vec3 B, vec3 E)
{
	Initialize(_nx, _ny, 0, 0, _x, _y);
	for (int i = 0; i <= nx; i++)
	{
		for (int j = 0; j <= ny; j++)
		{
			nodes[i][j].B = B;
			nodes[i][j].E = E;
		}
	}
}
Grid2d::Grid2d(int _nx, int _ny, double _ax, double _ay, double _bx, double _by)
{
	Initialize(_nx, _ny, _ax, _ay, _bx, _by);
}
Grid2d::Grid2d(int _nx, int _ny, double _ax, double _ay, double _bx, double _by, vec3 B, vec3 E)
{
	Initialize(_nx, _ny, _ax, _ay, _bx, _by);
	for (int i = 0; i <= nx; i++)
	{
		for (int j = 0; j <= ny; j++)
		{
			nodes[i][j].B = B;
			nodes[i][j].E = E;
		}
	}
}
Grid2d::Grid2d(const Grid2d &gr) : nx(gr.nx), ny(gr.ny), ax(gr.ax), ay(gr.ay), bx(gr.bx), by(gr.by), dx(gr.dx), dy(gr.dy)
{
	nodes = new node*[nx+1];
	for (int i = 0; i <= nx; i++)
	{
		nodes[i] = new node[ny+1];
		for (int j = 0; j <= ny; j++)
			nodes[i][j] = gr.nodes[i][j];
	}
}
void Grid2d::clearGrid()
{
	if (nodes)
	{
		for (int i = 0; i <= nx; i++)
			delete[] nodes[i];
	}
	delete[] nodes;
}
Grid2d::~Grid2d()
{
	clearGrid();
}

void Grid2d::Initialize(int _nx, int _ny, double _ax, double _ay, double _bx, double _by)
{
	nx = _nx; ny = _ny; ax = _ax; ay = _ay; bx = _bx; by = _by;

	nodes = new node*[nx + 1];
	for (int i = 0; i <= nx; i++)
	{
		nodes[i] = new node[ny + 1]();
	}

	dx = (bx - ax) / nx;
	dy = (by - ay) / ny;
}



Grid2d & Grid2d::operator=(const Grid2d & g)
{
	if (this == &g)
		return *this;
	clearGrid();
	nx = g.nx;
	ny = g.ny;
	ax = g.ax;
	bx = g.bx;
	ay = g.ay;
	by = g.by;
	dx = g.dx;
	dy = g.dy;
	nodes = new node*[nx + 1];
	for (int i = 0; i <= nx; i++)
	{
		nodes[i] = new node[ny + 1]();
		for (int j = 0; j <= ny; j++)
			nodes[i][j] = g.nodes[i][j];
	}
	return *this;
}

node &Grid2d::getRefNode(int i, int j) //убрать
{
	if (i < 0 || i > nx || j < 0 || j > ny) throw "bad index in getRefNode\n";
	return nodes[i][j];
}

vec4 Grid2d::getEx(vec2i v) const //Возвращает значение электрического поля (по оси OX) для 4 узлов
{
	return vec4(nodes[v.x][v.y].E.x, nodes[v.x][v.y+1].E.x, nodes[v.x+1][v.y].E.x, nodes[v.x+1][v.y+1].E.x);
}
vec4 Grid2d::getEy(vec2i v) const //Возвращает значение электрического поля (по оси OY) для 4 узлов
{
	return vec4(nodes[v.x][v.y].E.y, nodes[v.x][v.y + 1].E.y, nodes[v.x + 1][v.y].E.y, nodes[v.x + 1][v.y + 1].E.y);
}
vec4 Grid2d::getEz(vec2i v) const //Возвращает значение электрического поля (по оси OY) для 4 узлов
{
	return vec4(nodes[v.x][v.y].E.z, nodes[v.x][v.y + 1].E.z, nodes[v.x + 1][v.y].E.z, nodes[v.x + 1][v.y + 1].E.z);
}
vec4 Grid2d::getBx(vec2i v) const
{
	return vec4(nodes[v.x][v.y].B.x, nodes[v.x][v.y + 1].B.x, nodes[v.x + 1][v.y].B.x, nodes[v.x + 1][v.y + 1].B.x);
}
vec4 Grid2d::getBy(vec2i v) const
{
	return vec4(nodes[v.x][v.y].B.y, nodes[v.x][v.y + 1].B.y, nodes[v.x + 1][v.y].B.y, nodes[v.x + 1][v.y + 1].B.y);
}
vec4 Grid2d::getBz(vec2i v) const
{
	return vec4(nodes[v.x][v.y].B.z, nodes[v.x][v.y + 1].B.z, nodes[v.x + 1][v.y].B.z, nodes[v.x + 1][v.y + 1].B.z);
}

int Grid2d::gnx() const
{
	return nx;
}
int Grid2d::gny() const
{
	return ny;
}
double Grid2d::gdx() const
{
	return dx;
}
double Grid2d::gdy() const
{
	return dy;
}
double Grid2d::gax() const
{
	return ax;
}
double Grid2d::gay() const
{
	return ay;
}
double Grid2d::gbx() const
{
	return bx;
}
double Grid2d::gby() const
{
	return by;
}
bool Grid2d::inside(vec3 v) const
{
	return (v.x >= ax && v.x <= bx && v.y >= ay && v.y <= by);
}
vec2i Grid2d::getIndex(double x, double y) const
{
	if (x == bx) x = 0;
    if (y == by) y = 0;
	return vec2i((int)((x - ax) / dx), (int)((y - ay) / dy));
}
vec2 Grid2d::getLocalCoord(double x, double y) const
{
	vec2i coord = getIndex(x, y);
	return vec2(x / dx - ax / dx - coord.x, y / dy - ay / dy - coord.y);
}


