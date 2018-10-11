#include "grid3d.h"

Grid3d::Grid3d():nodes(){}

Grid3d::Grid3d(int _nx, int _ny, int _nz, double _ax, double _bx, double _ay, double _by, double _az, double _bz)
{
    Initialize(vec3<int>(_nx, _ny, _nz), vec3<double>(_ax, _ay, _az), vec3<double>(_bx, _by, _bz));
}
Grid3d::Grid3d(const Grid3d& gr)
{
    Initialize(gr.n, gr.a, gr.b);

    nodes = gr.nodes;
}
Grid3d::Grid3d(vec3<int> n, vec3<double> a, vec3<double> b)
{
    Initialize(n, a, b);
}
void Grid3d::clearGrid()
{
    nodes.Clear();
}

Grid3d::~Grid3d()
{
    clearGrid();
}


void Grid3d::Initialize(vec3<int> _n, vec3<double> _a, vec3<double> _b)
{
    clearGrid();

    n = _n; a = _a; b = _b;

    d = (b - a) / n;

    nodes.Initialize(n.x + 1, n.y + 1, n.z + 1);
}

int Grid3d::operator==(const Grid3d& gr) {
    if (a.x != gr.a.x) return 0;
    if (a.y != gr.a.y) return 0;
    if (a.z != gr.a.z) return 0;
    if (b.x != gr.b.x) return 0;
    if (b.y != gr.b.y) return 0;
    if (b.z != gr.b.z) return 0;

    return nodes == gr.nodes;
}

Grid3d& Grid3d::operator=(const Grid3d & gr)
{
    Initialize(gr.n, gr.a, gr.b);
    nodes = gr.nodes;
    return *this;
}

int Grid3d::gnxRealCells() const
{
    return n.x;
}
int Grid3d::gnyRealCells() const
{
    return n.y;
}
int Grid3d::gnzRealCells() const
{
    return n.z;
}

vec3<int> Grid3d::gnRealCells() const
{
    return n;
}

int Grid3d::gnxComplexCells() const
{
    return n.x;
}

int Grid3d::gnyComplexCells() const
{
    return n.y;
}

int Grid3d::gnzComplexCells() const
{
    return n.z/2+1;
}

vec3<int> Grid3d::gnComplexCells() const
{
    return vec3<int>(gnxComplexCells(), gnyComplexCells(), gnzComplexCells());
}

int Grid3d::gnxRealNodes() const
{
    return n.x + 1;
}
int Grid3d::gnyRealNodes() const
{
    return n.y + 1;
}
int Grid3d::gnzRealNodes() const
{
    return n.z + 1;
}

vec3<int> Grid3d::gnRealNodes() const
{
    return vec3<int>(gnxRealNodes(), gnyRealNodes(), gnzRealNodes());
}


double Grid3d::gdx() const
{
    return d.x;
}
double Grid3d::gdy() const
{
    return d.y;
}
double Grid3d::gdz() const
{
    return d.z;
}

vec3<double> Grid3d::gd() const
{
    return d;
}

double Grid3d::gax() const
{
    return a.x;
}
double Grid3d::gay() const
{
    return a.y;
}
double Grid3d::gaz() const
{
    return a.z;
}

vec3<double> Grid3d::ga() const
{
    return a;
}

double Grid3d::gbx() const
{
    return b.x;
}
double Grid3d::gby() const
{
    return b.y;
}
double Grid3d::gbz() const
{
    return b.z;
}

vec3<double> Grid3d::gb() const
{
    return b;
}

