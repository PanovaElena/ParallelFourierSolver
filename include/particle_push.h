#pragma once
class Grid2d;
class Particle;
struct vec3;
//�������������� ��������� �������� ������
void RelocateInside(double ax, double ay, double bx, double by, Particle &part);
void SetCurrentSpeedAndPos(const Grid2d &gr, Particle &p, double dt);
vec3 GetCurrentMomentum(const Grid2d &gr, Particle & part, double dt);//����� ����� � 2 ���� ������ �����������. ������� ����� ������