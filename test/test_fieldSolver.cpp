#include "gtest.h"
#include "field_solver.h"
#include "constants.h"
#include "particle.h"
#include "grid2d.h"

int mod(int a, int b);

TEST(test_field_solver, mod_is_right) {
	EXPECT_EQ(4, mod(-1, 5));//nx=5, 0 узел вычисляется на основе 1 и 4 узлов
	EXPECT_EQ(0, mod(0, 5));
	EXPECT_EQ(1, mod(1, 5));
	EXPECT_EQ(2, mod(2, 5));
	EXPECT_EQ(3, mod(3, 5));
	EXPECT_EQ(4, mod(4, 5));
	EXPECT_EQ(0, mod(5, 5));
	EXPECT_EQ(1, mod(6, 5));//nx=5, 5 узел вычисляется на основе 1 и 4 узлов
}

TEST(test_field_solver, right_calculating_E_if_B_is_null) {
	int nx = 4, ny = 4;
	Grid2d gr(nx, ny, 8, 8);

	for (int i = 0; i <= nx; i++)
		for (int j = 0; j <= ny; j++) {
			gr(i, j).B = vec3(0, 0, 0);
			gr(i, j).E = vec3(1, 2, 3);
			gr(i, j).J = vec3(i, j, i + j);
		}

	RefreshE(gr, 0.5);

	for (int i=0; i<=nx; i++)
		for (int j = 0; j <= ny; j++){
			EXPECT_DOUBLE_EQ(1 - 4 * constants::pi*0.5*i, gr(i, j).E.x);
			EXPECT_DOUBLE_EQ(2 - 4 * constants::pi*0.5*j, gr(i, j).E.y);
			EXPECT_DOUBLE_EQ(3 - 4 * constants::pi*0.5*(i+j), gr(i, j).E.z);
		}
}

TEST(test_field_solver, right_calculating_B_if_E_is_null) {
	int nx = 4, ny = 4;
	Grid2d gr(nx, ny, 8, 8);

	for (int i = 0; i <= nx; i++)
		for (int j = 0; j <= ny; j++) {
			gr(i, j).E = vec3(0, 0, 0);
			gr(i, j).B = vec3(1, 2, 3);
			gr(i, j).J = vec3(i, j, i + j);
		}

	RefreshB(gr, 0.5);

	for (int i = 0; i<=nx; i++)
		for (int j = 0; j <= ny; j++) {
			EXPECT_DOUBLE_EQ(1, gr(i, j).B.x);
			EXPECT_DOUBLE_EQ(2, gr(i, j).B.y);
			EXPECT_DOUBLE_EQ(3, gr(i, j).B.z);
		}
}

TEST(test_field_solver, right_calculating_E_without_board_if_I_is_null) {
	int nx = 4, ny = 4;
	Grid2d gr(nx, ny, 8, 8);

	for (int i = 0; i <= nx; i++)
		for (int j = 0; j <= ny; j++) {
			gr(i, j).B = vec3(mod(i, nx), mod(j, ny), mod(i+j,nx));//nx==ny
			gr(i, j).E = vec3(1, 2, 3);
			gr(i, j).J = vec3(0, 0, 0);
		}

	RefreshE(gr, 0.5);

	for (int i = 1; i < nx; i++)
		for (int j = 1; j < ny; j++) {
			EXPECT_DOUBLE_EQ(1 + constants::c*0.5*(gr(i, j + 1).B.z - gr(i, j - 1).B.z) / (gr.gdy() * 2), gr(i, j).E.x);
			EXPECT_DOUBLE_EQ(2 - constants::c*0.5*(gr(i + 1, j).B.z - gr(i - 1, j).B.z) / (gr.gdx() * 2), gr(i, j).E.y);
			EXPECT_DOUBLE_EQ(3 + constants::c*0.5*((gr(i + 1, j).B.y - gr(i - 1, j).B.y) / (gr.gdx() * 2) - (gr(i, j + 1).B.x - gr(i, j - 1).B.x) / (gr.gdy() * 2)), gr(i, j).E.z);
		}
}

TEST(test_field_solver, right_calculating_E_on_lower_board_if_I_is_null) {
	int nx = 4, ny = 4;
	Grid2d gr(nx, ny, 8, 8);

	for (int i = 0; i <= nx; i++)
		for (int j = 0; j <= ny; j++) {
			gr(i, j).B = vec3(mod(i, nx), mod(j, ny), mod(i + j, nx));//nx==ny
			gr(i, j).E = vec3(1, 2, 3);
			gr(i, j).J = vec3(0, 0, 0);
		}

	RefreshE(gr, 0.5);

	EXPECT_DOUBLE_EQ(1 + constants::c*0.5*(gr(0, 1).B.z - gr(0, ny-1).B.z) / (gr.gdy() * 2), gr(0, 0).E.x);
	EXPECT_DOUBLE_EQ(2 - constants::c*0.5*(gr(1, 0).B.z - gr(nx-1, 0).B.z) / (gr.gdx() * 2), gr(0, 0).E.y);
	EXPECT_DOUBLE_EQ(3 + constants::c*0.5*((gr(1, 0).B.y - gr(nx-1, 0).B.y) / (gr.gdx() * 2) - (gr(0, 1).B.x - gr(0, ny-1).B.x) / (gr.gdy() * 2)), gr(0, 0).E.z);

}

TEST(test_field_solver, right_calculating_E_on_upper_board_if_I_is_null) {
	int nx = 4, ny = 4;
	Grid2d gr(nx, ny, 8, 8);

	for (int i = 0; i <= nx; i++)
		for (int j = 0; j <= ny; j++) {
			gr(i, j).B = vec3(mod(i, nx), mod(j, ny), mod(i + j, nx));//nx==ny
			gr(i, j).E = vec3(1, 2, 3);
			gr(i, j).J = vec3(0, 0, 0);
		}

	RefreshE(gr, 0.5);

	EXPECT_DOUBLE_EQ(1 + constants::c*0.5*(gr(4, 1).B.z - gr(4, 3).B.z) / (gr.gdy() * 2), gr(4, 4).E.x);
	EXPECT_DOUBLE_EQ(2 - constants::c*0.5*(gr(1, 4).B.z - gr(3, 4).B.z) / (gr.gdx() * 2), gr(4, 4).E.y);
	EXPECT_DOUBLE_EQ(3 + constants::c*0.5*((gr(1, 4).B.y - gr(3, 4).B.y) / (gr.gdx() * 2) - (gr(4, 1).B.x - gr(4, 3).B.x) / (gr.gdy() * 2)), gr(4, 4).E.z);

}

TEST(test_field_solver, right_calculating_B_without_board) {
	int nx = 4, ny = 4;
	Grid2d gr(nx, ny, 8, 8);

	for (int i = 0; i <= nx; i++)
		for (int j = 0; j <= ny; j++) {
			gr(i, j).B = vec3(1,2,3);
			gr(i, j).E = vec3(mod(i, nx), mod(j, ny), mod(i + j, nx));//nx==ny
		}

	RefreshB(gr, 0.5);

	for (int i = 1; i<nx; i++)
		for (int j = 1; j < ny; j++) {
			EXPECT_DOUBLE_EQ(1 - constants::c*0.5*(gr(i, j + 1).E.z - gr(i, j - 1).E.z) / (gr.gdy() * 2), gr(i, j).B.x);
			EXPECT_DOUBLE_EQ(2 + constants::c*0.5*(gr(i + 1, j).E.z - gr(i - 1, j).E.z) / (gr.gdx() * 2), gr(i, j).B.y);
			EXPECT_DOUBLE_EQ(3 - constants::c*0.5*((gr(i + 1, j).E.y - gr(i - 1, j).E.y) / (gr.gdx() * 2) - (gr(i, j + 1).E.x - gr(i, j - 1).E.x) / (gr.gdy() * 2)), gr(i, j).B.z);
		}
}

TEST(test_field_solver, right_calculating_B_on_lower_board) {
	int nx = 4, ny = 4;
	Grid2d gr(nx, ny, 8, 8);

	for (int i = 0; i <= nx; i++)
		for (int j = 0; j <= ny; j++) {
			gr(i, j).B = vec3(1, 2, 3);
			gr(i, j).E = vec3(mod(i, nx), mod(j, ny), mod(i + j, nx));//nx==ny
		}

	RefreshB(gr, 0.5);

	EXPECT_DOUBLE_EQ(1 - constants::c*0.5*(gr(0, 1).E.z - gr(0, ny-1).E.z) / (gr.gdy() * 2), gr(0, 0).B.x);
	EXPECT_DOUBLE_EQ(2 + constants::c*0.5*(gr(1, 0).E.z - gr(nx-1, 0).E.z) / (gr.gdx() * 2), gr(0, 0).B.y);
	EXPECT_DOUBLE_EQ(3 - constants::c*0.5*((gr(1, 0).E.y - gr(nx-1, 0).E.y) / (gr.gdx() * 2) - (gr(0, 1).E.x - gr(0, ny-1).E.x) / (gr.gdy() * 2)), gr(0, 0).B.z);

}

TEST(test_field_solver, right_calculating_B_on_upper_board) {
	int nx = 4, ny = 4;
	Grid2d gr(nx, ny, 8, 8);

	for (int i = 0; i <= nx; i++)
		for (int j = 0; j <= ny; j++) {
			gr(i, j).B = vec3(1, 2, 3);
			gr(i, j).E = vec3(mod(i, nx), mod(j, ny), mod(i + j, nx));//nx==ny
		}

	RefreshB(gr, 0.5);

	EXPECT_DOUBLE_EQ(1 - constants::c*0.5*(gr(4, 1).E.z - gr(4, 3).E.z) / (gr.gdy() * 2), gr(4, 4).B.x);
	EXPECT_DOUBLE_EQ(2 + constants::c*0.5*(gr(1, 4).E.z - gr(3, 4).E.z) / (gr.gdx() * 2), gr(4, 4).B.y);
	EXPECT_DOUBLE_EQ(3 - constants::c*0.5*((gr(1, 4).E.y - gr(3, 4).E.y) / (gr.gdx() * 2) - (gr(4, 1).E.x - gr(4, 3).E.x) / (gr.gdy() * 2)), gr(4, 4).B.z);

}
