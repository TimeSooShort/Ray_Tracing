#pragma once

#include "ray.h"
#include "vec3.h"
#include "aabb.h"

class material;

struct hit_record
{
	point3 p; // ����������Ľ�������
	Vec3 normal; // ���㷨�������ǵ�λ����, ��������ϣ���������߷����෴
	double t; // ���߷��� p(t) = A + tb
	bool front_face; // true���������, false:���ڲ���

	// ������������Ĳ��ʣ����ڲ���ɢ��⼰��˥��ֵ
	std::shared_ptr<material> mat_ptr;

	// ��������u,v ��Χ [0, 1]
	double u;
	double v;

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