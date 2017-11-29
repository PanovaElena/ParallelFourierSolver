#include "plasma_system.h"
#include "current_deposition.h"
#include "field_interpolation.h"
#include "field_solver.h"
#include "particle_push.h"
#include <fstream>
#include <iostream>
#include <algorithm>

void PlasmaSystem::SetGrid(int MatrixSize_X, int MatrixSize_Y, double X_Min, double Y_Min, double X_Max, double Y_Max, funcField E, funcField B)
{
	Grid = Grid2d(MatrixSize_X, MatrixSize_Y, X_Min, Y_Min, X_Max, Y_Max);
	for (int i = 0; i <= MatrixSize_X; i++) {
		for (int j = 0; j <= MatrixSize_Y; j++) {
			Grid(i, j).E = E(DetX(i), DetY(j), 0, 0);
			Grid(i, j).B = B(DetX(i), DetY(j), 0, 0);
		}
	}
}
Particle PlasmaSystem::CreateParticle(int i, int j, double mass, double charge, double factor, funcVec3 v)
{
	double pcx = GenRand()*Grid.gdx() + DetX(i);
	double pcy = GenRand()*Grid.gdy() + DetY(j);
	return Particle(vec3(pcx, pcy, 0), v(pcx, pcy, 0), charge, mass, factor);
}
void PlasmaSystem::SetParticles(funcDistr D, double mass, double charge, double factor, funcVec3 V)
{
	//srand(time(0));
	srand(1);
	for (int j = 0; j < Grid.gny(); j++)
		for (int i = 0; i < Grid.gnx(); i++) {
			int nPart = (int)FindSUnderDistr(DetX(i), DetX(i + 1), DetY(j), DetY(j + 1), D);
			for (int k = 0; k < nPart; k++)
			{
				Particle p = CreateParticle(i, j, mass, charge, factor, V);
				Particles.push_back(p);
			}
		}
	NParticles = Particles.size();
	MoveParticlesIntoGrid();
}
void PlasmaSystem::RunModel(double dt, int iterationsNumber, int iterationsBetweenDumps, bool isPrint, std::string fileOutputDirectory)
{
	double sp = 0.0;
	if (*(fileOutputDirectory.end() - 1) != '/') fileOutputDirectory += '/';
	for (int i = 0; i < iterationsNumber; i++)
	{
		if (isPrint && i % iterationsBetweenDumps == 0)
			Print(fileOutputDirectory, i, 1000);
		for (int j = 0; j < NParticles; j++)
		{
			SetCurrentSpeedAndPos(Grid, Particles[j], dt);
		}

		CurrentDeposition(Grid, Particles);
		FieldSolver(Grid, dt);
	}
}
double PlasmaSystem::FindSUnderDistr(double x1, double x2, double y1, double y2, funcDistr D)
{
	double dy = 0.001, dx = dy;
	int n = (int)((x2 - x1) / dx);
	int m = (int)((y2 - y1) / dy);
	double s = 0;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			s += D(x1 + i*dx, y1 + j*dy, 0)*dx*dy;
	return s;
}


std::vector<double> operator/ (const std::vector<double>& v, int d) {
	std::vector<double> v1(v.size());
	for (int i = 0; i < v.size(); i++)
		v1[i] = v[i]/ d;
	return v1;
}
std::vector<double> PlasmaSystem::CalculateNumParticleInRows(int minRow, int maxRow)
{
	if (minRow == maxRow) return std::vector<double> (Grid.gnx());

	std::vector<double> v(Grid.gnx());
	for (const Particle& part : Particles)
	{
		vec2i coord = Grid.getIndex(part.getC().x, part.getC().y);
		if (coord.y >= minRow && coord.y < maxRow)
			v[coord.x]++;
	}

	return v/(maxRow-minRow);
}




void PlasmaSystem::Print(std::string outDir, int timeStep, int particleStep)
{
	PrintGrid(outDir, timeStep, E);
	PrintGrid(outDir, timeStep, B);
	PrintParticles(outDir, "particles", timeStep, V, particleStep);
}
void PlasmaSystem::PrintParticle(std::ofstream & f, const std::string ident, int numPart, bool nextRow)
{
	for (int k = 0; k < 3; k++)
		if (ident == C)
			f << Particles[numPart].getC()[k] << ";";
		else if (ident == V)
			f << Particles[numPart].getV()[k] << ";";
	if (nextRow)
		f << '\n';
	else
		f << ';';
}
void PlasmaSystem::PrintParticles(std::string &outDir, const std::string &type, int timeStep, const std::string &ident, int particleStep)
{
	particleStep = std::max(1, particleStep);
	std::ofstream fPart;
	openStream(fPart, outDir, type, timeStep, ident, true);
	for (int i = 0; i < Particles.size(); i += particleStep)
		PrintParticle(fPart, ident, i, i >= Particles.size() - particleStep);
}
void PlasmaSystem::PrintRowGrid(std::ofstream &f, int indRow, int indCoord, const std::string &ident)
{
	f << indRow << ";";
	for (int j = 0; j <= Grid.gny(); j++)
		if (ident == B)
			f << Grid(indRow, j).B[indCoord] << ";";
		else if (ident == E)
			f << Grid(indRow, j).E[indCoord] << ";";
	f << '\n';
}
void PlasmaSystem::PrintGrid(std::string &outDir, int timeStep, const std::string &ident)
{
	std::ofstream fGrid;
	openStream(fGrid, outDir, "grid", timeStep, ident, true);
	char ar[] = { 'x', 'y', 'z' };
	for (int indexCoord = 0; indexCoord < 3; indexCoord++)
	{
		fGrid << ar[indexCoord] << ';';
		for (int i = 0; i <= Grid.gny(); i++) 
				fGrid << i << ';';
		fGrid << '\n';
		for (int i = 0; i <= Grid.gnx(); i++)
			PrintRowGrid(fGrid, i, indexCoord, ident);
		fGrid << '\n';
	}
	fGrid.close();
}
void PlasmaSystem::openStream(std::ofstream &f, std::string &outDir, const std::string &type, int timeStep, const std::string &ident, bool createNewFile)
{
	if (*(outDir.end() - 1) != '/')
		outDir += '/';
	if (createNewFile)
		f.open(outDir + type + "_" + (timeStep == -1 ? std::to_string(timeStep):"") +"_" + ident + ".csv");
	else f.open(outDir + type + "_"  + (timeStep == -1 ? std::to_string(timeStep) : "") + "_" + ident + ".csv", std::ios::app);
}



