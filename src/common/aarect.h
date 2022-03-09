#pragma once

#include "hittable.h"
#include "material.h"

class xy_rect : public hittable
{
public:
	xy_rect() = default;
	xy_rect(double x_0, double x_1, double y_0, double y_1, double k_, std::shared_ptr<material> m) :
		x0(x_0), x1(x_1), y0(y_0), y1(y_1), k(k_), mat(m) {}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

	virtual bool bounding_box(double _time0, double _time1, aabb& output_box) const override
	{
		output_box = aabb(point3(x0, y0, k - aarect_z_ln), point3(x1, y1, k + aarect_z_ln));
		return true;
	}

	std::shared_ptr<material> mat;
	double x0, x1, y0, y1, k;
};

class xz_rect : public hittable
{
public:
	xz_rect() = default;
	xz_rect(double x_0, double x_1, double z_0, double z_1, double k_, std::shared_ptr<material> m) :
		x0(x_0), x1(x_1), z0(z_0), z1(z_1), k(k_), mat(m) {}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double _time0, double _time1, aabb& output_box) const override
	{
		output_box = aabb(point3(x0, k - aarect_z_ln, z0), point3(x1, k + aarect_z_ln, z1));
		return true;
	}

	std::shared_ptr<material> mat;
	double x0, x1, z0, z1, k;
};

class yz_rect : public hittable
{
public:
	yz_rect() = default;
	yz_rect(double y_0, double y_1, double z_0, double z_1, double k_, std::shared_ptr<material> m) :
		y0(y_0), y1(y_1), z0(z_0), z1(z_1), k(k_), mat(m) {}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double _time0, double _time1, aabb& output_box) const override
	{
		output_box = aabb(point3(k - aarect_z_ln, y0, z0), point3(k + aarect_z_ln, y1, z1));
		return true;
	}

	std::shared_ptr<material> mat;
	double y0, y1, z0, z1, k;
};

// 计算交点
// 根据射线公式的其映射在各个坐标轴上的方程：P(t) = A + tb
// 由于z是固定值(k)，所以先用z轴方程计算初t的值，再计算出x, y
bool xy_rect::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	auto t = (k - r.origin().z()) / r.direction().z();
	if (t < t_min || t > t_max)
	{
		return false;
	}
	auto x = r.origin().x() + t * r.direction().x();
	auto y = r.origin().y() + t * r.direction().y();

	if (x < x0 || x > x1 || y < y0 || y > y1)
	{
		return false;
	}
	rec.u = (x - x0) / (x1 - x0);
	rec.v = (y - y0) / (y1 - y0);
	rec.t = t;
	rec.set_face_normal(r, Vec3(0, 0, 1));
	rec.mat_ptr = mat;
	rec.p = r.at(t);
	return true;
}

bool xz_rect::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	auto t = (k - r.origin().y()) / r.direction().y();
	if (t < t_min || t > t_max)
	{
		return false;
	}
	auto x = r.origin().x() + t * r.direction().x();
	auto z = r.origin().z() + t * r.direction().z();

	if (x < x0 || x > x1 || z < z0 || z > z1)
	{
		return false;
	}
	rec.u = (x - x0) / (x1 - x0);
	rec.v = (z - z0) / (z1 - z0);
	rec.t = t;
	rec.set_face_normal(r, Vec3(0, 1, 0));
	rec.mat_ptr = mat;
	rec.p = r.at(t);
	return true;
}

bool yz_rect::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	auto t = (k - r.origin().x()) / r.direction().x();
	if (t < t_min || t > t_max)
	{
		return false;
	}
	auto y = r.origin().y() + t * r.direction().y();
	auto z = r.origin().z() + t * r.direction().z();

	if (y < y0 || y > y1 || z < z0 || z > z1)
	{
		return false;
	}
	rec.u = (y - y0) / (y1 - y0);
	rec.v = (z - z0) / (z1 - z0);
	rec.t = t;
	rec.set_face_normal(r, Vec3(1, 0, 0));
	rec.mat_ptr = mat;
	rec.p = r.at(t);
	return true;
}


