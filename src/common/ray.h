#pragma once

#include "vec3.h"

class ray
{
public:
	ray() = default;
	ray(const point3& origin, const Vec3& direction, double time = 0.0)
		: orig(origin), dir(direction), tm(time)
	{}

	point3 origin() const { return orig; }
	Vec3 direction() const { return dir; }
	double time() const { return tm; }

	// 射线方程：P(t) = A + tb，A 为起点，b为射线方向
	// 返回一个点
	point3 at(double t) const
	{
		return orig + t * dir;
	}

private:
	point3 orig; // 射线起点坐标
	Vec3 dir; // 射线方向
	double tm{}; // 给射线加上时间
};
