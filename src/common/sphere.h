#pragma once

#include "hittable.h"

class sphere : public hittable
{
public:
	sphere() = default;
	sphere(point3 cen, double r, std::shared_ptr<material> ptr) 
		: center(cen), radius(r), mat_ptr(ptr) {}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double _time0, double _time1, aabb& output_box) const override;

	point3 center; // 球心坐标
	double radius; // 半径
	std::shared_ptr<material> mat_ptr;

private:
	// u,v是纹理坐标，范围是[0,1]，关键在于如何将球面坐标(x, y, z)映射为(u, v)？
	// 假设球心在原点的单位圆，弧度a 代表与-y轴的夹角，范围是[0, π]，弧度b 代表与-x轴的夹角，方向是 -x -> +z -> x -> -z -> -x，范围是[0, 2π]
	// u = b/(2π), v = a/π，球面点坐标(x, y, z) = (-cos(b)sin(a), -cos(a), sin(b)sina(a))，利用反三角函数得 a = acos(-y)，b = atan2(z, -x)，
	// 由于atan2 的变化是从0到π再反转过来从-π到0，则u就变成了[0, 1/2] 再从 [-1/2, 0]，这不符合要求，于是加π为了保持值不变参数取反(atan2(q,w) = atan2(-q,-w))
	// 所以 b = atan2(-z, x)+π
	// 这里p是单位球面上的点，满足于条件 x*x + y*y + z*z = 1 的点都可作为球面点
	static void get_sphere_uv(const point3& p, double& u, double& v)
	{
		auto theta = std::acos(-p.y());
		auto phi = atan2(-p.z(), p.x()) + PI;

		u = phi / (2 * PI);
		v = theta / PI;
	}
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	Vec3 oc = r.origin() - center;
	auto a = r.direction().length_squared();
	auto half_b = dot(oc, r.direction());
	auto c = oc.length_squared() - radius * radius;
	auto discriminant = half_b * half_b - a * c;
	if (discriminant < 0)
	{
		return false;
	}
	auto sqrt = std::sqrt(discriminant);

	// 找出范围内最近交点
	// ------------------
	// 射线在圆外，则(-half_b - sqrt) / a 值最小，在园内则只有一个交点
	// 所以最近点射线方程参数是(-half_b - sqrt) / a，加入范围考虑
	auto minT = (-half_b - sqrt) / a;
	if (minT < t_min || minT > t_max)
	{
		minT = (-half_b + sqrt) / a;
		if (minT < t_min || minT > t_max)
		{
			return false;
		}
	}

	rec.t = minT;
	rec.p = r.at(minT);
	Vec3 outward_normal = (rec.p - center) / radius;
	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mat_ptr;
	get_sphere_uv(outward_normal, rec.u, rec.v);
	return true;
}

// 球的轴对齐边界框
bool sphere::bounding_box(double _time0, double _time1, aabb& output_box) const
{
	output_box = aabb(center - Vec3(radius, radius, radius), center + Vec3(radius, radius, radius));
	return true;
}