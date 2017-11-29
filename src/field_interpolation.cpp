#include "field_interpolation.h"
#include "recalc_coord.h"
#include "grid2d.h"
#include "particle.h"

vec3 calcE(const Grid2d & gr, const Particle& p)
{
	double dx = gr.gdx(), dy = gr.gdy(), ax = gr.gax(), ay = gr.gay();
	vec2i ind = gr.getIndex(p.getC().x, p.getC().y);
	double x = GetNewX(p.getC().x, ind.x, ax, dx);
	double y = GetNewY(p.getC().y, ind.y, ay, dy);
	vec4 Ex = gr.getEx(ind);
	vec4 Ey = gr.getEy(ind);
	vec4 Ez = gr.getEz(ind);

	return vec3((1 - x)*(1 - y)*Ex.a1 + (1 - x)*y*Ex.a2 + x*(1 - y)*Ex.a3 + x*y*Ex.a4,
		(1 - x)*(1 - y)*Ey.a1 + (1 - x)*y*Ey.a2 + x*(1 - y)*Ey.a3 + x*y*Ey.a4, 
		(1 - x)*(1 - y)*Ez.a1 + (1 - x)*y*Ez.a2 + x*(1 - y)*Ez.a3 + x*y*Ez.a4);
}

vec3 calcB(const Grid2d & gr, const Particle& p)
{
	double dx = gr.gdx(), dy = gr.gdy(), ax = gr.gax(), ay = gr.gay();
	vec2i ind = gr.getIndex(p.getC().x, p.getC().y);
	double x = GetNewX(p.getC().x, ind.x, ax, dx);
	double y = GetNewY(p.getC().y, ind.y, ay, dy);
	vec4 Bx = gr.getBx(ind);
	vec4 By = gr.getBy(ind);
	vec4 Bz = gr.getBz(ind);
	return vec3((1 - x)*(1 - y)*Bx.a1 + (1 - x)*y*Bx.a2 + x*(1 - y)*Bx.a3 + x*y*Bx.a4,
		(1 - x)*(1 - y)*By.a1 + (1 - x)*y*By.a2 + x*(1 - y)*By.a3 + x*y*By.a4, 
		(1 - x)*(1 - y)*Bz.a1 + (1 - x)*y*Bz.a2 + x*(1 - y)*Bz.a3 + x*y*Bz.a4);
}

