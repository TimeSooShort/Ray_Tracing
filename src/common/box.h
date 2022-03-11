#pragma once

#include "hittable_list.h"
#include "aarect.h"

class box : public hittable
{
public:
	box() = default;
	box(const point3& p0_min, const point3& p1_max, std::shared_ptr<material> mat);

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override
	{
		return sides.hit(r, t_min, t_max, rec);
	}

	virtual bool bounding_box(double _time0, double _time1, aabb& output_box) const override
	{
		output_box = aabb(box_min, box_max);
		return true;
	}

	point3 box_min;
	point3 box_max;
	hittable_list sides;
};

box::box(const point3& p0_min, const point3& p1_max, std::shared_ptr<material> mat)
{
	box_min = p0_min;
	box_max = p1_max;

	sides.add(std::make_shared<xy_rect>(p0_min.x(), p1_max.x(), p0_min.y(), p1_max.y(), p0_min.z(), mat));
	sides.add(std::make_shared<xy_rect>(p0_min.x(), p1_max.x(), p0_min.y(), p1_max.y(), p1_max.z(), mat));

	sides.add(std::make_shared<xz_rect>(p0_min.x(), p1_max.x(), p0_min.z(), p1_max.z(), p0_min.y(), mat));
	sides.add(std::make_shared<xz_rect>(p0_min.x(), p1_max.x(), p0_min.z(), p1_max.z(), p0_min.y(), mat));

	sides.add(std::make_shared<yz_rect>(p0_min.y(), p1_max.y(), p0_min.z(), p1_max.z(), p0_min.x(), mat));
	sides.add(std::make_shared<yz_rect>(p0_min.y(), p1_max.y(), p0_min.z(), p1_max.z(), p0_min.x(), mat));

}