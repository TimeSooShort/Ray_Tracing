#pragma once

#include "hittable.h"

#include <vector>
#include <memory>

class hittable_list : public hittable
{
public:
	hittable_list() = default;
	hittable_list(std::shared_ptr<hittable> object)
	{
		add(object);
	}

	void add(std::shared_ptr<hittable> object)
	{
		objs.push_back(object);
	}
	void clear()
	{
		objs.clear();
	}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double _time0, double _time1, aabb& output_box) const override;

public:
	std::vector<std::shared_ptr<hittable>> objs;
};

bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	hit_record tmp_rec;
	bool hit_anything = false;
	auto closet_so_far = t_max;

	for (const auto& obj : objs)
	{
		if (obj->hit(r, t_min, closet_so_far, tmp_rec))
		{
			hit_anything = true;
			closet_so_far = tmp_rec.t;
			rec = tmp_rec;
		}
	}
	return hit_anything;
}

bool hittable_list::bounding_box(double _time0, double _time1, aabb& output_box) const
{
	aabb tmp;
	bool first = true;
	for (const auto& obj : objs)
	{
		if (!obj->bounding_box(_time0, _time1, tmp))
		{
			return false;
		};
		if (first)
		{
			output_box = tmp;
			first = false;
		}
		else 
		{
			output_box = surrounding_box(tmp, output_box);
		}
	}
	return true;
}