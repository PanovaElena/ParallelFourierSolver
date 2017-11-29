#pragma once
class Grid2d;
class Particle;
struct vec3;
#include <vector>

//����������� ��������� ����, ����������� ��������
vec3 getJ(const Particle& p, double dx, double dy);
//����������� �����
void CurrentDeposition(Grid2d& gr, const Particle& p);
void CurrentDeposition(Grid2d& gr, const std::vector<Particle>& particles);//������� ����� ��������� ����� ������
void SumIOnBoard(Grid2d& gr);