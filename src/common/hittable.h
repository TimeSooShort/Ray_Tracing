#pragma once

#include "ray.h"
#include "vec3.h"
#include "aabb.h"

class material;

struct hit_record
{
	point3 p; // ����������Ľ�������
	Vec3 normal; // ���㷨�������ǵ�λ����, ��������ϣ���������߷����෴
	double t{}; // ���߷��� p(t) = A + tb
	bool front_face{}; // true���������, false:���ڲ���

	// ������������Ĳ��ʣ����ڲ���ɢ��⼰��˥��ֵ
	std::shared_ptr<material> mat_ptr;

	// ��������u,v ��Χ [0, 1]
	double u{};
	double v{};

	// outward_normal ������ָ�򽻵㷽��ķ�������
	// ����������outward_normal�����Ƿ���ͬ�ж����⣬����normal����ֵ
	void set_face_normal(const ray& r, const Vec3& outward_normal)
	{
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class hittable
{
public:
	// t_min, t_max Ϊɸѡ��Χ
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
	// ���ظ������aabb
	virtual bool bounding_box(double _time0, double _time1, aabb& output_box) const = 0;
};

class translate : public hittable
{
public:
	translate(std::shared_ptr<hittable> p, const Vec3& displacement)
		: ptr(p), offset(displacement) {}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double _time0, double _time1, aabb& output_box) const override;

	std::shared_ptr<hittable> ptr;
	Vec3 offset;
};

class rotate_y : public hittable
{
public:
	// angle ��ת�Ƕ�
	rotate_y(std::shared_ptr<hittable> p, double angle);

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double _time0, double _time1, aabb& output_box) const override
	{
		output_box = bbox;
		return hasbox;
	}

	std::shared_ptr<hittable> ptr; // ָ����ת�ĺ���
	double sin_theta{}; // ��ת�Ƕȵ�sinֵ
	double cos_theta{}; // ��ת�Ƕȵ�cosֵ
	bool hasbox{};
	aabb bbox; // ��Χ��ת��ĺ��ӵĺ��ӣ��ú��������
};

bool translate::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	ray moved_r(r.origin() - offset, r.direction(), r.time());
	if (!ptr->hit(r, t_min, t_max, rec))
	{
		return false;
	}
	rec.p += offset;
	return true;
}

bool translate::bounding_box(double _time0, double _time1, aabb& output_box) const
{
	if (!ptr->bounding_box(_time0, _time1, output_box))
	{
		return false;
	}
	output_box = aabb(output_box.min() + offset, output_box.max() + offset);
	return true;
}

rotate_y::rotate_y(std::shared_ptr<hittable> p, double angle)
{
	auto radians = degrees_to_radians(angle);
	sin_theta = sin(radians);
	cos_theta = cos(radians);
	hasbox = ptr->bounding_box(0, 1, bbox);

	if (!hasbox)
	{
		return;
	}

	point3 min(infinity, infinity, infinity);
	point3 max(-infinity, -infinity, -infinity);

	// ����Ŀ��������˸����㣬�ҵ���ת��������min����Զ��max�����¹�����ת����ӵĺ���
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				auto x = i * bbox.max().x() + (1 - i) * bbox.min().x();
				auto y = j * bbox.max().y() + (1 - j) * bbox.min().y();
				auto z = k * bbox.max().z() + (1 - k) * bbox.min().z();

				auto x_new = cos_theta * x + sin_theta * z;
				auto z_new = -sin_theta * x + cos_theta * z;;

				Vec3 tmp(x_new, y, z_new);
				for (int c = 0; c < 3; c++)
				{
					min[c] = fmin(min[c], tmp[c]);
					max[c] = fmax(max[c], tmp[c]);
				}
			}
		}
	}
	bbox = aabb(min, max);
}

bool rotate_y::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	auto origin = r.origin();
	auto direction = r.direction();

	// �����߷�����ת
	origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
	origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

	direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
	direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

	ray rotate_r(origin, direction, r.time());

	if (!ptr->hit(rotate_r, t_min, t_max, rec))
	{
		return false;
	}

	// ��ʱ�Ľ�����δ��תǰ�������ϵĽ���,��������ת�õ��ý�����ת���λ��
	auto p = rec.p;
	auto normal = rec.normal;

	p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
	p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

	normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
	normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

	rec.p = p;
	rec.set_face_normal(rotate_r, normal);

	return true;
}


