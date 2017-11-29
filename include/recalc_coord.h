#pragma once

inline double GetNewX(double oldX, int cell, double ax, double dx)
{
	return  oldX / dx - ax / dx - cell;
}
inline double GetNewY(double oldY, int cell, double ay, double dy)
{
	return oldY / dy - ay / dy - cell;
}