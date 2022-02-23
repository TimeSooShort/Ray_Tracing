#pragma once

#include "hittable.h"

class sphere : public hittable
{
public:
	sphere() = default;
	sphere(point3 cen, double r, std::shared_ptr<material> ptr) 
		: center(cen), radius(r), mat_ptr(ptr) {}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double _time0, double _time1, aabb& output_box) const override;

	point3 center; // ��������
	double radius; // �뾶
	std::shared_ptr<material> mat_ptr;

private:
	// u,v���������꣬��Χ��[0,1]���ؼ�������ν���������(x, y, z)ӳ��Ϊ(u, v)��
	// ����������ԭ��ĵ�λԲ������a ������-y��ļнǣ���Χ��[0, ��]������b ������-x��ļнǣ������� -x -> +z -> x -> -z -> -x����Χ��[0, 2��]
	// u = b/(2��), v = a/�У����������(x, y, z) = (-cos(b)sin(a), -cos(a), sin(b)sina(a))�����÷����Ǻ����� a = acos(-y)��b = atan2(z, -x)��
	// ����atan2 �ı仯�Ǵ�0�����ٷ�ת������-�е�0����u�ͱ����[0, 1/2] �ٴ� [-1/2, 0]���ⲻ����Ҫ�����ǼӦ�Ϊ�˱���ֵ�������ȡ��(atan2(q,w) = atan2(-q,-w))
	// ���� b = atan2(-z, x)+��
	// ����p�ǵ�λ�����ϵĵ㣬���������� x*x + y*y + z*z = 1 �ĵ㶼����Ϊ�����
	static void get_sphere_uv(const point3& p, double& u, double& v)
	{
		auto theta = std::acos(-p.y());
		auto phi = atan2(-p.z(), p.x()) + PI;

		u = phi / (2 * PI);
		v = theta / PI;
	}
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	Vec3 oc = r.origin() - center;
	auto a = r.direction().length_squared();
	auto half_b = dot(oc, r.direction());
	auto c = oc.length_squared() - radius * radius;
	auto discriminant = half_b * half_b - a * c;
	if (discriminant < 0)
	{
		return false;
	}
	auto sqrt = std::sqrt(discriminant);

	// �ҳ���Χ���������
	// ------------------
	// ������Բ�⣬��(-half_b - sqrt) / a ֵ��С����԰����ֻ��һ������
	// ������������߷��̲�����(-half_b - sqrt) / a�����뷶Χ����
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
	Vec3 outward_normal = (rec.p - center) / radius;
	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mat_ptr;
	get_sphere_uv(outward_normal, rec.u, rec.v);
	return true;
}

// ��������߽��
bool sphere::bounding_box(double _time0, double _time1, aabb& output_box) const
{
	output_box = aabb(center - Vec3(radius, radius, radius), center + Vec3(radius, radius, radius));
	return true;
}