#include "dataReader.h"

void readConstgrwork(Grid2d &gr, int &nx, int &ny, double &ax, double &ay, double &bx, double &by, double &dt, int &numIteration)
{
	cin >> nx >> ny >> ax >> ay >> bx >> by >> dt >> numIteration;
	gr = Grid2d(nx,ny,ax,ay,bx,by);
}
void readGrid2d(Grid2d & gr)
{
	for (int i = 0; i <= gr.gnx(); i++)
		for (int j = 0; j <= gr.gny(); j++)
			cin >> gr.getRefNode(i, j).E.x >> gr.getRefNode(i, j).E.y;

	for (int i = 0; i <= gr.gnx(); i++)
		for (int j = 0; j <= gr.gny(); j++)
			cin >> gr.getRefNode(i, j).B.x >> gr.getRefNode(i, j).B.y;
}

void readConstParticles(vector<Particle> &v, int &num)
{
	cin >> num;
	v.resize(num);
}
void readParticles2d(vector<Particle>& v)
{
	//m q Coord V
	for (auto part : v)
	{
		double m;
		cin >> m;
		part.setM(m);
	}

	for (auto part : v)
	{
		double q;
		cin >> q;
		part.setQ(q);
	}

	vec3 vect(0, 0, 0);
	//почему 2 раза считываются координаты?
	//потому что неправильно)
	for (auto part : v) {
		cin >> vect.x >> vect.y;
		part.setC(vect);
	}

	for (auto part : v) {
		cin >> vect.x >> vect.y;
		part.setV(vect);
	}
}

void readAll(Grid2d &gr, vector<Particle> &v, int &nx, int &ny, double &ax, double &ay, double &bx, double &by, double &dt, int &numIteration, int &numPart)
{
	readConstgrwork(gr, nx, ny, ax, ay, bx, by, dt, numIteration);
	readGrid2d(gr);
	
	readConstParticles(v, numPart);
	readParticles2d(v);
}

