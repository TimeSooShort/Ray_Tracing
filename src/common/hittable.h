#pragma once

#include "ray.h"
#include "vec3.h"
#include "aabb.h"

class material;

struct hit_record
{
	point3 p; // 射线与物体的交点坐标
	Vec3 normal; // 交点法向量，是单位向量, 这里我们希望其与射线方向相反
	double t; // 射线方程 p(t) = A + tb
	bool front_face; // true：球外侧面, false:球内测面

	// 光线所射物体的材质，用于产生散射光及光衰减值
	std::shared_ptr<material> mat_ptr;

	// 纹理坐标u,v 范围 [0, 1]
	double u;
	double v;

	// outward_normal 是球心指向交点方向的法线向量
	// 根据射线与outward_normal方向是否相同判断内外，决定normal最终值
	void set_face_normal(const ray& r, const Vec3& outward_normal)
	{
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class hittable
{
public:
	// t_min, t_max 为筛选范围
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
	// 返回该物体的aabb
	virtual bool bounding_box(double _time0, double _time1, aabb& output_box) const = 0;
};