#pragma once
#include "grid2d.h"
#include "particle.h"
#include "vec.h"
#include <vector>
#include <ctime>
#include <string>

//дл€ печати в файл
const std::string C = "coord";
const std::string V = "speed";
const std::string B = "B";
const std::string E = "E";

typedef vec3(*funcField)(double x, double y, double z, double t);
typedef double(*funcDistr)(double x, double y, double z);
typedef vec3(*funcVec3)(double x, double y, double z);

void RelocateInside(double ax, double ay, double bx, double by, Particle &part);

inline vec3 NullFunc(double x, double y, double z) {
	return vec3(0, 0, 0);
}
inline vec3 NullFunc(double x, double y, double z, double t) {
	return vec3(0, 0, 0);
}

class PlasmaSystem {
protected:
	std::vector<Particle> Particles;
	long long NParticles;
	Grid2d Grid;
public:
	void SetGrid(int MatrixSize_X, int MatrixSize_Y, double X_Min, double Y_Min, double X_Max, double Y_Max, funcField E, funcField B);

	//создает частицы случайным образом в i,j €чейке с заданной скоростью - функцией координат
	Particle CreateParticle(int i, int j, double mass, double charge, double factor, funcVec3 v);

	//создает одинаковые частицы в сетке, использу€ функцию распределени€
	void SetParticles(funcDistr D, double mass, double charge, double factor, funcVec3 V = NullFunc);

	//запускает процесс моделировани€
	void RunModel(double dt, int iterationNumber, int iterationsBetweenDumps, bool isPrint, std::string fileOutputDirectory="");

protected:
	double DetX(int i) {
		return Grid.gax() + i*Grid.gdx();
	}
	double DetY(int j) {
		return Grid.gay() + j*Grid.gdy();
	}

	virtual double FindSUnderDistr(double x1, double x2, double y1, double y2, funcDistr D);

	double GenRand() {//[0,1]
		return ((double)rand()) / (double)RAND_MAX;
	}
	void MoveParticlesIntoGrid() {
		for (Particle p : Particles)
			RelocateInside(Grid.gax(), Grid.gay(), Grid.gbx(), Grid.gby(), p);
	}

	std::vector<double> CalculateNumParticleInRows(int minRow, int maxRow);

	virtual void Print(std::string outDir, int timeStep, int particleStep);
	void PrintParticle(std::ofstream &f, const std::string ident, int numPart, bool nextRow);
	void PrintParticles(std::string &outDir, const std::string &type, int timeStep, const std::string &ident, int particleStep);
	void PrintRowGrid(std::ofstream &f, int indRow, int indCoord, const std::string &ident);
	void PrintGrid(std::string &outDir, int timeStep, const std::string &ident);

	void openStream(std::ofstream &f, std::string &outDir, const std::string &type, int timeStep, const std::string &ident, bool createNewFile);
};
