#include "gtest.h"
#include "vec.h"
#include "my_complex.h"
#include "_double.h"

TEST(TestVector, vector_product1) 
{
	vec3<Double> a = vec3<Double>(6,7,10);
	vec3<Double> b = vec3<Double>(8, 5, 9);
	vec3<Double> res = a*b;
	EXPECT_EQ(res.x(),13);
	EXPECT_EQ(res.y(),26 );
	EXPECT_EQ(res.z(),-26);
}
TEST(TestVector, vector_product2)
{
	vec3<Double> a = vec3<Double>(-1, 2, -2);
	vec3<Double> b = vec3<Double>(2, 1, -1);
	vec3<Double> res = a*b;
	EXPECT_EQ(res.x(), 0);
	EXPECT_EQ(res.y(), -5);
	EXPECT_EQ(res.z(), -5);
}
TEST(TestVector, vector_product3)
{
	vec3<Double> a = vec3<Double>(2, 1, -3);
	vec3<Double> b = vec3<Double>(0, -1, 1);
	vec3<Double> res = a*b;
	EXPECT_EQ(res.x(), -2);
	EXPECT_EQ(res.y(), -2);
	EXPECT_EQ(res.z(), -2);
}
TEST(TestVector, vector_product4)
{
	vec3<Double> a = vec3<Double>(2, -3, 1);
	vec3<Double> b = vec3<Double>(-4, -2, 7);
	vec3<Double> res = a*b;
	EXPECT_EQ(res.x(), -19);
	EXPECT_EQ(res.y(), -18);
	EXPECT_EQ(res.z(), -16);
}
TEST(TestVector, vector_length1)
{
	vec3<Double> a = vec3<Double>(-4, 0, 3);
	EXPECT_EQ(a.getNorm(), 5);
}
TEST(TestVector, vector_length2)
{
	vec3<Double> a = vec3<Double>(0, 0, 1);
	EXPECT_EQ(a.getNorm(), 1);
}
TEST(TestVector, vector_length3)
{
	vec3<Double> a = vec3<Double>(2, 2, 2);
	EXPECT_EQ(a.getNorm(), sqrt(12));
}
TEST(TestVector, scalar_product1)
{
	vec3<Double> a = vec3<Double>(1, 2, 3);
	a = a * 3;
	vec3<Double> b = vec3<Double>(3, 6, 9);
	EXPECT_TRUE(a.x() == b.x() && a.y() == b.y() && a.z() == b .z());
}
TEST(TestVector, scalar_product2)
{
	vec3<Double> a = vec3<Double>(0, 0, 0);
	a = a * 3;
	vec3<Double> b = vec3<Double>(0, 0, 0);
	EXPECT_TRUE(a.x() == b.x() && a.y() == b.y() && a.z() == b.z());
}
TEST(TestVector, scalar_product3)
{
	vec3<Double> a = vec3<Double>(1, 1, 1);
	a = a * 0;
	vec3<Double> b = vec3<Double>(0, 0, 0);
	EXPECT_TRUE(a.x() == b.x() && a.y() == b.y() && a.z() == b.z());
}
TEST(TestVector, scalar_product4)
{
	vec3<Double> a = vec3<Double>(1, 1, 1);
	a = a  +  a;
	vec3<Double> b = vec3<Double>(2, 2, 2);
	EXPECT_TRUE(a.x() == b.x() && a.y() == b.y() && a.z() == b.z());
}

TEST(TestVector, scalar_product_double) {
    vec3<Double> a(1, 2, 3), b(3, 2, 1);
    ASSERT_EQ(vec3<Double>::ScalarProduct(a, b), 10);
}

TEST(TestVector, scalar_product_complex) {
    vec3<MyComplex> a(complex_i, complex_i, complex_i);
    vec3<MyComplex> b(complex_i, complex_i, complex_i);
    ASSERT_EQ(vec3<MyComplex>::ScalarProduct(a, b), 3);
}

TEST(TestVector, vector_product_double) {
    vec3<Double> a(1, 0, 0), b(0, 1, 0);
    ASSERT_EQ(vec3<Double>::VectorProduct(a, b), vec3<Double>(0, 0, 1));
}

TEST(TestVector, vector_product_complex) {
    vec3<MyComplex> a(complex_i, complex_0, complex_0);
    vec3<MyComplex> b(complex_0, complex_i, complex_0);
    ASSERT_EQ(vec3<MyComplex>::VectorProduct(a, b), vec3<MyComplex>(complex_0, complex_0, MyComplex(1,0)));
}

TEST(TestVector, normalize_double) {
    vec3<Double> a(1, 1, 1);
    ASSERT_EQ(a.Normalize(), vec3<Double>(1 / sqrt(3), 1 / sqrt(3), 1 / sqrt(3)));
}


