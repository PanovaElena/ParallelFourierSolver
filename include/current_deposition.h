#pragma once
class Grid2d;
class Particle;
struct vec3;
#include <vector>

//определение плотности тока, создаваемой частицей
vec3 getJ(const Particle& p, double dx, double dy);
//взвешивание токов
void CurrentDeposition(Grid2d& gr, const Particle& p);
void CurrentDeposition(Grid2d& gr, const std::vector<Particle>& particles);//считаем сразу суммарный вклад частиц
void SumIOnBoard(Grid2d& gr);