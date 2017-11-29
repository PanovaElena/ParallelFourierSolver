#pragma once
class Grid2d;
class Particle;
struct vec3;
//интегрирование уравнения движения частиц
void RelocateInside(double ax, double ay, double bx, double by, Particle &part);
void SetCurrentSpeedAndPos(const Grid2d &gr, Particle &p, double dt);
vec3 GetCurrentMomentum(const Grid2d &gr, Particle & part, double dt);//ответ ровно в 2 раза меньше правильного. кажется нашел ошибку