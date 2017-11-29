#pragma once
struct vec3;
class Grid2d;
class Particle;

//интерполяция
vec3 calcE(const Grid2d &gr, const Particle& p);//вычисляет значение напряженности поля в точке расположения частицы 
vec3 calcB(const Grid2d &gr, const Particle& p);