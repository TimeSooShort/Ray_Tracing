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

	// ���߷��̣�P(t) = A + tb��A Ϊ��㣬bΪ���߷���
	// ����һ����
	point3 at(double t) const
	{
		return orig + t * dir;
	}

private:
	point3 orig; // �����������
	Vec3 dir; // ���߷���
	double tm{}; // �����߼���ʱ��
};
