#pragma once
#include <iostream>
#include <vector>

#include "grid2d.h"
#include "particle.h"

using std::vector;
using std::cout;
using std::cin;

void readConstgrwork(Grid2d &gr, int &nx, int &ny, double &ax, double &ay, double &bx, double &by, double &dt, int &numIteration);
void readGrid2d(Grid2d &gr);
void readConstParticles(vector<Particle> &v, int &num);
void readParticles2d(vector<Particle> &v);

void readAll(Grid2d &gr, vector<Particle> &v, int &nx, int &ny, double &ax, double &ay, double &bx, double &by, double &dt, int &numIteration, int &numPart);