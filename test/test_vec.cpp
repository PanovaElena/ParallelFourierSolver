#include "gtest.h"
#include "vec.h"

TEST(test_vector, vector_product1) 
{
	vec3 a = vec3(6,7,10);
	vec3 b = vec3(8, 5, 9);
	vec3 res = a*b;
	EXPECT_EQ(res.x,13);
	EXPECT_EQ(res.y,26 );
	EXPECT_EQ(res.z,-26);
}
TEST(test_vector, vector_product2)
{
	vec3 a = vec3(-1, 2, -2);
	vec3 b = vec3(2, 1, -1);
	vec3 res = a*b;
	EXPECT_EQ(res.x, 0);
	EXPECT_EQ(res.y, -5);
	EXPECT_EQ(res.z, -5);
}
TEST(test_vector, vector_product3)
{
	vec3 a = vec3(2, 1, -3);
	vec3 b = vec3(0, -1, 1);
	vec3 res = a*b;
	EXPECT_EQ(res.x, -2);
	EXPECT_EQ(res.y, -2);
	EXPECT_EQ(res.z, -2);
}
TEST(test_vector, vector_product4)
{
	vec3 a = vec3(2, -3, 1);
	vec3 b = vec3(-4, -2, 7);
	vec3 res = a*b;
	EXPECT_EQ(res.x, -19);
	EXPECT_EQ(res.y, -18);
	EXPECT_EQ(res.z, -16);
}
TEST(test_vector, vector_length1)
{
	vec3 a = vec3(-4, 0, 3);
	EXPECT_EQ(a.getLength(), 5);
}
TEST(test_vector, vector_length2)
{
	vec3 a = vec3(0, 0, 1);
	EXPECT_EQ(a.getLength(), 1);
	EXPECT_EQ(a.getLength(), a.getSqLength());
}
TEST(test_vector, vector_length3)
{
	vec3 a = vec3(2, 2, 2);
	EXPECT_EQ(a.getSqLength(), 12);
}
TEST(test_vector, scalar_product1)
{
	vec3 a = vec3(1, 2, 3);
	a = a * 3;
	vec3 b = vec3(3, 6, 9);
	EXPECT_TRUE(a.x == b.x && a.y == b.y && a.z == b .z);
}
TEST(test_vector, scalar_product2)
{
	vec3 a = vec3(0, 0, 0);
	a = a * 3;
	vec3 b = vec3(0, 0, 0);
	EXPECT_TRUE(a.x == b.x && a.y == b.y && a.z == b.z);
}
TEST(test_vector, scalar_product3)
{
	vec3 a = vec3(1, 1, 1);
	a = a * 0;
	vec3 b = vec3(0, 0, 0);
	EXPECT_TRUE(a.x == b.x && a.y == b.y && a.z == b.z);
}
TEST(test_vector, scalar_product4)
{
	vec3 a = vec3(1, 1, 1);
	a = a  +  a;
	vec3 b = vec3(2, 2, 2);
	EXPECT_TRUE(a.x == b.x && a.y == b.y && a.z == b.z);
}