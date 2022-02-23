#pragma once

#include "ray.h"

// axis-aligned bounding boxes  轴对齐的边界框
class aabb
{
public:
	aabb() = default;
	aabb(const point3& a, const point3& b) : minimum(a), maximum(b) {}

	aabb(const aabb& box) : minimum(box.min()), maximum(box.max()) {}

	aabb& operator=(const aabb& box)
	{
		minimum = box.min();
		maximum = box.max();
		return *this;
	}

	point3 min() const { return minimum; }
	point3 max() const { return maximum; }

	bool hit(const ray& r, double t_min, double t_max) const;

private:
	point3 minimum;
	point3 maximum;
};

/*
* 判断射线是否击中盒子
射线公式 P(t)=A+tb，映射在x轴得 x(t)=Ax+tbx，如果射线击中 x = x0 面，则 x0=Ax+t0bx，得 t0=(x0-Ax)/bx，
同理 x1 面可得 t1=(x1-Ax)/bx，我们规定 tx0 小于 tx1，则公式最终为 tx0=min((x0-Ax)/bx,(x1-Ax)/bx)， tx1=max((x0-Ax)/bx,(x1-Ax)/bx)
对x/y/z 分别求出各自t的范围，如果有交集则说明射线经过盒子。
*/
inline bool aabb::hit(const ray& r, double t_min, double t_max) const
{
	for (int a = 0; a < 3; a++)
	{
		auto invD = 1.0f / r.direction()[a];
		auto t0 = (min()[a] - r.origin()[a]) * invD;
		auto t1 = (max()[a] - r.origin()[a]) * invD;
		if (invD < 0.0)
		{
			std::swap(t0, t1);
		}
		t_min = t0 > t_min ? t0 : t_min;
		t_max = t1 < t_max ? t1 : t_max;
		if (t_max <= t_min)
		{
			return false;
		}
	}
	return true;
}

// 返回包围住这两个盒子的外围大盒子
inline aabb surrounding_box(aabb box0, aabb box1)
{
	point3 small(std::fmin(box0.min().x(), box1.min().x()),
		std::fmin(box0.min().y(), box1.min().y()),
		std::fmin(box0.min().z(), box1.min().z()));

	point3 big(std::fmax(box0.max().x(), box1.max().x()),
		std::fmax(box0.max().y(), box1.max().y()),
		std::fmax(box0.max().z(), box1.max().z()));

	return aabb(small, big);
}

//inline bool box_compare(const aabb& a, const aabb& b, const aabb_axi& axis)
//{
//	aabb_axi_type i = static_cast<aabb_axi_type>(axis);
//	return a.min().e[i] < b.min().e[i];
//}