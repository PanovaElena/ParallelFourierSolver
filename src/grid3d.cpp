#include "grid3d.h"

Grid3d::Grid3d():nodes(0){}

Grid3d::Grid3d(int _nx, int _ny, int _nz, double _ax, double _bx, double _ay, double _by, double _az, double _bz)
{
	Initialize(_nx, _ny, _nz, _ax, _bx, _ay, _by, _az, _bz);
}
Grid3d::Grid3d(const Grid3d& gr)
{
	Initialize(gr.nx, gr.ny, gr.nz, gr.ax, gr.bx, gr.ay, gr.by, gr.az, gr.bz);

	for (int i = 0; i < (nx + 1)*(ny + 1)*(nz + 1); i++)
		(*this)(i) = gr(i);
}
void Grid3d::clearGrid()
{
	if (nodes)
	{
		delete[] tmp1;
		delete[] tmp2;
		delete[] nodes;
	}
}

Grid3d::~Grid3d()
{
	clearGrid();
}


void Grid3d::Initialize(int _nx, int _ny, int _nz, double _ax, double _bx, double _ay, double _by, double _az, double _bz)
{
	nx = _nx; ny = _ny; nz = _nz; ax = _ax; ay = _ay; az = _az; bx = _bx; by = _by; bz = _bz;

	tmp1 =new node[(nx + 1)*(ny + 1)*(nz + 1)];
	tmp2 = new node*[(nx + 1)*(ny + 1)];
	nodes = new node**[(nx + 1)];

	for (int i = 0; i < (nx + 1)*(ny + 1); i++)
		tmp2[i] = tmp1 + i*(nz + 1);
	for (int i = 0; i < nx + 1; i++)
		nodes[i] = tmp2 + i*(ny + 1);

	dx = (bx - ax) / nx;
	dy = (by - ay) / ny;
	dz = (bz - az) / nz;
}

int Grid3d::operator==(const Grid3d& gr) {
	if (nx != gr.nx) return 0;
	if (ny != gr.ny) return 0;
	if (nz != gr.nz) return 0;
	if (ax != gr.ax) return 0;
	if (ay != gr.ay) return 0;
	if (az != gr.az) return 0;
	if (bx != gr.bx) return 0;
	if (by != gr.by) return 0;
	if (bz != gr.bz) return 0;

	for (int i = 0; i < (nx + 1)*(ny + 1)*(nz + 1); i++) {
		if ((*this)(i).B != gr(i).B) return 0;
		if ((*this)(i).E != gr(i).E) return 0;
	}
	return 1;

}

void Grid3d::operator=(const Grid3d & gr)
{
	if (nx != gr.nx) return;
	if (ny != gr.ny) return;
	if (nz != gr.nz) return;
	if (ax != gr.ax) return;
	if (ay != gr.ay) return;
	if (az != gr.az) return;
	if (bx != gr.bx) return;
	if (by != gr.by) return;
	if (bz != gr.bz) return;

	for (int i = 0; i < (nx + 1)*(ny + 1)*(nz + 1); i++)
		(*this)(i) = gr(i);
	
}

int Grid3d::gnxCells() const
{
	return nx;
}
int Grid3d::gnyCells() const
{
	return ny;
}
int Grid3d::gnzCells() const
{
	return nz;
}

int Grid3d::gnxNodes() const
{
	return nx + 1;
}
int Grid3d::gnyNodes() const
{
	return ny + 1;
}
int Grid3d::gnzNodes() const
{
	return nz + 1;
}

int Grid3d::gnxComplexNodes() const
{
	return nx + 1;
}
int Grid3d::gnyComplexNodes() const
{
	return ny + 1;
}
int Grid3d::gnzComplexNodes() const
{
	return (nz + 1) / 2 + 1;
}

double Grid3d::gdx() const
{
	return dx;
}
double Grid3d::gdy() const
{
	return dy;
}
double Grid3d::gdz() const
{
	return dz;
}

double Grid3d::gax() const
{
	return ax;
}
double Grid3d::gay() const
{
	return ay;
}
double Grid3d::gaz() const
{
	return az;
}

double Grid3d::gbx() const
{
	return bx;
}
double Grid3d::gby() const
{
	return by;
}
double Grid3d::gbz() const
{
	return bz;
}

