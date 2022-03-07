#pragma once

#include <math.h>
#include <iostream>
#include "const_var.h"

class Vec3
{
public:
	Vec3() : e{0, 0, 0} {}
	Vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}
	Vec3(const Vec3& v) : e{ v.x(), v.y(), v.z() } {}

	Vec3& operator=(const Vec3& v)
	{
		e[0] = v.x();
		e[1] = v.y();
		e[2] = v.z();
		return *this;
	}

	double x() const;
	double y() const;
	double z() const;

	double& operator[](int i);
	const double& operator[](int i) const;

	Vec3 operator-() const;
	Vec3& operator+=(const Vec3& v);
	Vec3& operator*=(const double t);
	Vec3& operator/=(const double t);
	double length() const;
	double length_squared() const;

	inline static Vec3 random(double min = 0.0, double max = 1.0)
	{
		return Vec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}

	bool near_zero() const
	{
		return (std::fabs(e[0]) < minest_length_dimensions) && 
			(std::fabs(e[1]) < minest_length_dimensions) && 
			(std::fabs(e[2]) < minest_length_dimensions);
	}
public:
	double e[3]{};
};

using point3 = Vec3;
using color = Vec3;

inline std::ostream& operator<<(std::ostream& out, const  Vec3& v)
{
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline  Vec3 operator+(const  Vec3& u, const  Vec3& v)
{
	return  Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline  Vec3 operator-(const  Vec3& u, const  Vec3& v)
{
	return  Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline  Vec3 operator*(const  Vec3& u, const  Vec3& v)
{
	return  Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline  Vec3 operator*(double t, const  Vec3& v)
{
	return  Vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline  Vec3 operator*(const  Vec3& v, double t)
{
	return t * v;
}

inline  Vec3 operator/(const  Vec3& v, double t)
{
	return (1 / t) * v;
}

inline double dot(const  Vec3& u, const  Vec3& v)
{
	return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline  Vec3 cross(const  Vec3& u, const  Vec3& v) {
	return  Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
		u.e[2] * v.e[0] - u.e[0] * v.e[2],
		u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline Vec3 unit_vector(Vec3 v)
{
	return v / v.length();
}

// ����һ����������λ���ĵ����Ÿ����������ڲ������ĳ��
// ������������������p���õ��������ģ�����������������p���ķ�������
inline Vec3 random_in_unit_sphere()
{
	double theta = acos(2 * random_double() - 1);
	double phi = 2 * random_double() * PI;

	double x = cos(phi) * sin(theta);
	double z = sin(phi) * sin(theta);
	double y = cos(theta);

	return { x, y, z };
	/*while (true)
	{
		auto p = Vec3::random(-1, 1);
		if (p.length_squared() >= 1)
		{
			continue;
		}
		return p;
	}*/
}

// Lambertian Reflection
inline Vec3 random_unit_vector()
{
	return random_in_unit_sphere();
	//return unit_vector(random_in_unit_sphere());
}

// ���ص���������ͷ��߱���ͬһ����
inline Vec3 random_in_hemisphere(const Vec3& normal)
{
	Vec3 in_unit_sphere = random_in_unit_sphere();
	if (dot(in_unit_sphere, normal) > 0.0)
	{
		return in_unit_sphere;
	}
	return -in_unit_sphere;
}

// �������v�ķ���
inline Vec3 reflect(const Vec3& v, const Vec3& n)
{
	// v��n�����෴�����Բ��ü���
	return v - 2 * dot(v, n) * n;
}

// �����������.  ˹��������:v*sin a=v'*sin a', v����������
// һ�����ֵ�ʵ�������ǣ�������λ�������ʽϸߵĲ�����ʱ��˹�ζ�����û�������Ľ⣬��˲����ܷ�������
// ������ߴӲ�����1.5�������������1.0�������ݹ�ʽ sin a' = 1.5/1.0 sin a�����̿����޽⣬��Ϊsina'�����ڴ���1��ֵ
// �ƶϳ�������ߵ�ˮƽ����ֱ������R'��=v/v'*(R+cosa * n); R'|| = -sqrt(1-pow(|R'��|, 2))*n
// ������� R �ͷ��� n ���ǵ�λ���������� R'��=v/v'*(R+(-R*n)n)
// etai_over_etat = v/v', uv ��������ߣ� n�Ƿ����������ǵ�λ����
inline Vec3 refract(const Vec3& uv, const Vec3& n, double etai_over_etat)
{
	auto cos_theta = std::fmin(dot(-uv, n), 1.0); // ����ʹ��fminӦ���ǿ��ǵ����㾫�ȵ����⣬ȷ��dot������ᳬ��1.0
	Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	Vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
	return r_out_perp + r_out_parallel;
}

// ����ɢ��ģ��(����)���ڹ�Ȧ���������һ����Ϊorigin
inline Vec3 random_in_unit_disk() {
	while (true) {
		auto p = Vec3(random_double(-1, 1), random_double(-1, 1), 0);
		if (p.length_squared() >= 1) continue;
		return p;
	}
}