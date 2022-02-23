#pragma once

#include "hittable.h"
#include "vec3.h"
#include "material.h"

class moving_sphere : public hittable
{
public:
	moving_sphere() {}
	moving_sphere(point3 cen0, point3 cen1, double tm0, double tm1, double r, std::shared_ptr<material> m)
		: center0(cen0), center1(cen1), time0(tm0), time1(tm1), radius{ r }, mat_ptr(m)
	{}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

	point3 center(double time) const;
public:
	point3 center0, center1;
	double time0{}, time1{};
	double radius{};
	std::shared_ptr<material> mat_ptr = nullptr;
};

point3 moving_sphere::center(double time) const
{
	return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

bool moving_sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	Vec3 oc = r.origin() - center(r.time());
	auto a = r.direction().length_squared();
	auto half_b = dot(oc, r.direction());
	auto c = oc.length_squared() - radius * radius;
	auto discriminant = half_b * half_b - a * c;
	if (discriminant < 0)
	{
		return false;
	}
	auto sqrt = std::sqrt(discriminant);

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
	rec.set_face_normal(r, (rec.p - center(r.time())) / radius);
	rec.mat_ptr = mat_ptr;

	return true;
}

bool moving_sphere::bounding_box(double _time0, double _time1, aabb& output_box) const
{
	aabb box0 = aabb(center(_time0) - Vec3(radius, radius, radius),
		center(_time0) + Vec3(radius, radius, radius));

	aabb box1 = aabb(center(_time1) - Vec3(radius, radius, radius),
		center(_time1) + Vec3(radius, radius, radius));

	output_box = surrounding_box(box0, box1);
	return true;
}