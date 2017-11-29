#include "dataReader.h"
#include "dateWriter.h"
#include "particle_push.h"
#include "current_deposition.h"
#include "field_solver.h"
#include <string>

int main()
{
	Grid2d gr;
	vector<Particle> v;
	double dt, ax, ay, bx, by;
	int numIteration, nx, ny, numParticles;

	//freopen("testIN.txt", "r", stdin);
	//freopen("testOUT.txt", "w", stdout);

	readAll(gr, v, nx, ny, ax, ay, bx, by, dt, numIteration, numParticles);

	for (int i = 0; i < numIteration; i++)
	{
		//этап 1 расчет силы Лоренца, скорости и определение новго местоположения
		for (Particle p : v)
			SetCurrentSpeedAndPos(gr, p, dt);

		//этап 2 расчет возникающих токов

		CurrentDeposition(gr, v);

		//этап 3 расчет поля в узлах
		FieldSolver(gr, dt);
	}
	return 0;
}