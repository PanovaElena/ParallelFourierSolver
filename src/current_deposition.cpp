#include "current_deposition.h"
#include "recalc_coord.h"
#include "grid2d.h"
#include "particle.h"

vec3 getJ(const Particle& p, double dx, double dy){
	return vec3(p.getQ()*p.getV().x / (dx*dy), p.getQ()*p.getV().y / (dx*dy), p.getQ()*p.getV().z / (dx*dy));
}

void SetToZeroJ(Grid2d& gr) {
	for (int i = 0; i <= gr.gnx(); i++)
		for (int j = 0; j <= gr.gny(); j++)
			gr.getRefNode(i, j).J = vec3(0, 0, 0);
}

void CurrentDeposition(Grid2d& gr, const Particle& p) {
	vec3 I = getJ(p, gr.gdx(), gr.gdy());
	vec2i ind = gr.getIndex(p.getC().x,p.getC().y);
	double x = GetNewX(p.getC().x, ind.x, gr.gax(), gr.gdx());
	double y = GetNewY(p.getC().y, ind.y, gr.gay(), gr.gdy());
	gr.getRefNode(ind.x, ind.y).J += I*(1 - x)*(1 - y);
	gr.getRefNode(ind.x, ind.y + 1).J += I*(1 - x)*y;
	gr.getRefNode(ind.x + 1, ind.y).J += I*x*(1 - y);
	gr.getRefNode(ind.x + 1, ind.y + 1).J += I*x*y;
}

void CurrentDeposition(Grid2d& gr, const std::vector<Particle>& particles)
{
	SetToZeroJ(gr);

	for (int i = 0; i < particles.size(); i++)
	{
		CurrentDeposition(gr, particles[i]);
	}

	SumIOnBoard(gr);
}

void SumIOnBoard(Grid2d & gr)
{
	for (int i = 0; i <= gr.gnx(); i++) {
		gr(i, 0).J += gr(i, gr.gny()).J;
		gr(i, gr.gny()).J = gr(i, 0).J;
	}
	for (int i = 0; i <= gr.gny(); i++) {
		gr(0, i).J += gr(gr.gnx(), i).J;
		gr(gr.gnx(), i).J = gr(0, i).J;
	}
}
