#include "gtest.h"
#include "vec.h"
#include "my_complex.h"

TEST(TestVector, vector_product1) 
{
	vec3<double> a = vec3<double>(6,7,10);
	vec3<double> b = vec3<double>(8, 5, 9);
	vec3<double> res = a*b;
	EXPECT_EQ(res.x(),13);
	EXPECT_EQ(res.y(),26 );
	EXPECT_EQ(res.z(),-26);
}
TEST(TestVector, vector_product2)
{
	vec3<double> a = vec3<double>(-1, 2, -2);
	vec3<double> b = vec3<double>(2, 1, -1);
	vec3<double> res = a*b;
	EXPECT_EQ(res.x(), 0);
	EXPECT_EQ(res.y(), -5);
	EXPECT_EQ(res.z(), -5);
}
TEST(TestVector, vector_product3)
{
	vec3<double> a = vec3<double>(2, 1, -3);
	vec3<double> b = vec3<double>(0, -1, 1);
	vec3<double> res = a*b;
	EXPECT_EQ(res.x(), -2);
	EXPECT_EQ(res.y(), -2);
	EXPECT_EQ(res.z(), -2);
}
TEST(TestVector, vector_product4)
{
	vec3<double> a = vec3<double>(2, -3, 1);
	vec3<double> b = vec3<double>(-4, -2, 7);
	vec3<double> res = a*b;
	EXPECT_EQ(res.x(), -19);
	EXPECT_EQ(res.y(), -18);
	EXPECT_EQ(res.z(), -16);
}
TEST(TestVector, vector_length1)
{
	vec3<double> a = vec3<double>(-4, 0, 3);
	EXPECT_EQ(a.getNorm(), 5);
}
TEST(TestVector, vector_length2)
{
	vec3<double> a = vec3<double>(0, 0, 1);
	EXPECT_EQ(a.getNorm(), 1);
}
TEST(TestVector, vector_length3)
{
	vec3<double> a = vec3<double>(2, 2, 2);
	EXPECT_EQ(a.getNorm(), sqrt(12));
}
TEST(TestVector, scalar_product1)
{
	vec3<double> a = vec3<double>(1, 2, 3);
	a = a * 3;
	vec3<double> b = vec3<double>(3, 6, 9);
	EXPECT_TRUE(a.x() == b.x() && a.y() == b.y() && a.z() == b .z());
}
TEST(TestVector, scalar_product2)
{
	vec3<double> a = vec3<double>(0, 0, 0);
	a = a * 3;
	vec3<double> b = vec3<double>(0, 0, 0);
	EXPECT_TRUE(a.x() == b.x() && a.y() == b.y() && a.z() == b.z());
}
TEST(TestVector, scalar_product3)
{
	vec3<double> a = vec3<double>(1, 1, 1);
	a = a * 0;
	vec3<double> b = vec3<double>(0, 0, 0);
	EXPECT_TRUE(a.x() == b.x() && a.y() == b.y() && a.z() == b.z());
}
TEST(TestVector, scalar_product4)
{
	vec3<double> a = vec3<double>(1, 1, 1);
	a = a  +  a;
	vec3<double> b = vec3<double>(2, 2, 2);
	EXPECT_TRUE(a.x() == b.x() && a.y() == b.y() && a.z() == b.z());
}

TEST(TestVector, scalar_product_double) {
    vec3<double> a(1, 2, 3), b(3, 2, 1);
    ASSERT_EQ(vec3<double>::ScalarProduct(a, b), 10);
}

TEST(TestVector, scalar_product_complex) {
    vec3<MyComplex> a(complex_i, complex_i, complex_i);
    vec3<MyComplex> b(complex_i, complex_i, complex_i);
    ASSERT_EQ(vec3<MyComplex>::ScalarProduct(a, b), MyComplex(3,0));
}

TEST(TestVector, vector_product_double) {
    vec3<double> a(1, 0, 0), b(0, 1, 0);
    ASSERT_EQ(vec3<double>::VectorProduct(a, b), vec3<double>(0, 0, 1));
}

TEST(TestVector, vector_product_complex) {
    vec3<MyComplex> a(complex_i, complex_0, complex_0);
    vec3<MyComplex> b(complex_0, complex_i, complex_0);
    ASSERT_EQ(vec3<MyComplex>::VectorProduct(a, b), vec3<MyComplex>(complex_0, complex_0, MyComplex(1,0)));
}

TEST(TestVector, vector_product_complex_2) {
    vec3<MyComplex> a(complex_i, complex_i*(-1), complex_i);
    vec3<MyComplex> b(complex_i*(-1), complex_i, complex_i*(-1));
    ASSERT_EQ(vec3<MyComplex>::VectorProduct(a, b), vec3<MyComplex>(complex_0, complex_0, complex_0));
}

TEST(TestVector, vector_product_complex_vectors_are_orthogonal) {
    vec3<MyComplex> a(MyComplex(1, 2), MyComplex(4, 7), MyComplex(1, 3));
    vec3<MyComplex> b(MyComplex(3, 9), MyComplex(2, 2), MyComplex(6, 7));
    vec3<MyComplex> c = vec3<MyComplex>::VectorProduct(a, b);
    ASSERT_EQ(complex_0, vec3<MyComplex>::ScalarProduct(a, c));
    ASSERT_EQ(complex_0, vec3<MyComplex>::ScalarProduct(b, c));
}

TEST(TestVector, decomposition_of_vector_into_components) {
    vec3<MyComplex> E(MyComplex(1, 2), MyComplex(4, 7), MyComplex(1, 3));
    vec3<MyComplex> K(MyComplex(3, 9), MyComplex(2, 2), MyComplex(6, 7));
    vec3<MyComplex> k = K.Normalize();
    vec3<MyComplex> El = k*vec3<MyComplex>::ScalarProduct(E, k);
    vec3<MyComplex> kE = vec3<MyComplex>::VectorProduct(k, E);
    vec3<MyComplex> Et = (vec3<MyComplex>::VectorProduct(k, kE))*(-1);

    ASSERT_EQ(E, (El + Et));
}

TEST(TestVector, normalize_double) {
    vec3<double> a(1, 1, 1);
    ASSERT_EQ(a.Normalize(), vec3<double>(1 / sqrt(3), 1 / sqrt(3), 1 / sqrt(3)));
}

TEST(TestVector, normalize_complex) {
    vec3<MyComplex> a(MyComplex(1,0), MyComplex(1, 1), MyComplex(0, 1));
    ASSERT_EQ(a.Normalize(), a*(1.0/sqrt(2+sqrt(2))));
}


