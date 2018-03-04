#include "gtest.h"
#include "grid3d.h"

TEST(TestGrid, created_correctly) {
	ASSERT_NO_THROW(Grid3d;);
}

TEST(TestGrid, created_correctly_with_parameters) {
	ASSERT_NO_THROW(Grid3d(10,20,30,1,2,3,11,22,33););
}

TEST(TestGrid, index) {
	Grid3d gr(10, 20, 30, 1, 2, 3, 11, 22, 33);
	ASSERT_TRUE(gr(0,10,20).E==Node().E);
}

TEST(TestGrid, copy_constructor) {
	Grid3d grid(10, 20, 30, 1, 2, 3, 11, 22, 33);
	Grid3d grid2 = grid;
	ASSERT_TRUE(grid2 == grid);
}

TEST(TestGrid, copy) {
	Grid3d grid(10, 20, 30, 1, 2, 3, 11, 22, 33);
	Grid3d grid3(10, 20, 30, 1, 2, 3, 11, 22, 33);
	grid3 = grid;
	ASSERT_TRUE(grid3 == grid);
}