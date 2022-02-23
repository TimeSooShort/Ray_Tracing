#include "vec3.h"
#include <iostream>

double Vec3::x() const { return e[0]; }
double Vec3::y() const { return e[1]; }
double Vec3::z() const { return e[2]; }

double& Vec3::operator[](int i) { return e[i]; }
const double& Vec3::operator[](int i) const { return e[i]; }

Vec3 Vec3::operator-() const { return Vec3(-e[0], -e[1], -e[2]); }

Vec3& Vec3::operator+=(const Vec3& v)
{
	e[0] += v.e[0];
	e[1] += v.e[1];
	e[2] += v.e[2];
	return *this;
}

Vec3& Vec3::operator*=(const double t)
{
	e[0] *= t;
	e[1] *= t;
	e[2] *= t;
	return *this;
}

Vec3& Vec3::operator/=(const double t)
{
	return *this *= 1 / t;
}

double Vec3::length() const
{
	return sqrt(length_squared());
}

double Vec3::length_squared() const
{
	return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
}

