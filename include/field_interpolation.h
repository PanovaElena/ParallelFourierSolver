#pragma once
struct vec3;
class Grid2d;
class Particle;

//������������
vec3 calcE(const Grid2d &gr, const Particle& p);//��������� �������� ������������� ���� � ����� ������������ ������� 
vec3 calcB(const Grid2d &gr, const Particle& p);