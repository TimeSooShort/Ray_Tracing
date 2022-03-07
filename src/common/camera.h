#pragma once

#include "const_var.h"
#include "vec3.h"

class camera
{
public:
	camera() = default;
	// lookfrom:�����λ�ã�lookat���������׼�ĵ㣬vup ���������е��Ϸ���vfov����ֱ�����ӽ��� aspect_ratio����߱�
	// aperture: ��Ȧ��focus_dist���۽�ƽ��(focus plane),��ƽ���ϵ����ж��������������ľ۽�״̬
	camera(point3 lookfrom, point3 lookat, Vec3 vup, double vfov, double aspect_ratio, 
		double aperture, double focus_dist, double tm0 = 0.0, double tm1 = 0.0)
	{
		auto theta = degrees_to_radians(vfov);
		auto h = std::tan(theta / 2);
		//auto aspect_ratio = 16.0 / 9.0; // ��/�� ��
		auto viewport_height = 2.0 * h; // �Ӵ��߶�
		auto viewport_width = aspect_ratio * viewport_height;
		//auto focal_length = 1.0; // ����

		// ��������ϵ
		w = unit_vector(lookfrom-lookat); // ���������ķ���
		u = unit_vector(cross(vup, w)); // ���������
		v = cross(w, u); // ��������Ϸ�

		origin = lookfrom;
		horizontal = focus_dist * viewport_width * u;
		vertical = focus_dist * viewport_height * v;
		lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

		lens_radius = aperture / 2;
		time0 = tm0;
		time1 = tm1;
	}

	camera(const camera& cam);
	camera& operator=(const camera& cam);
	virtual ~camera() {}

	ray get_ray(double s, double t) const
	{
		//return ray(origin, lower_left_corner+s*horizontal+t*vertical-origin);
		// �ڹ�Ȧ�ڲ���һ����������������ƶ�ԭorigin��
		Vec3 rd = lens_radius * random_in_unit_disk();
		Vec3 offset = u * rd.x() + v * rd.y();

		return ray(
			origin + offset,
			lower_left_corner + s * horizontal + t * vertical - origin - offset,
			random_double(time0, time1)
		);
	}
public:
	point3 origin; // �����λ��
	point3 lower_left_corner; // �Ӵ����½����꣬������ǵ㲻������
	Vec3 horizontal; // ˮƽ������������С�����Ӵ���ȣ������ƶ�����
	Vec3 vertical; // ��ֱ������������С�����Ӵ��߶ȣ������ƶ�����
	Vec3 u, v, w; // �������ϵ
	double lens_radius{}; // ��Ȧ
	double time0{}, time1{}; // ��������ŵĿ���ʱ�䣬�������ʱ���������ѡȡĳ�̷�������
};

camera::camera(const camera& cam) : origin(cam.origin), lower_left_corner(cam.lower_left_corner), horizontal(cam.horizontal),
	vertical(cam.vertical), u(cam.u), v(cam.v), w(cam.w), lens_radius(cam.lens_radius), time0(cam.time0), time1(cam.time1)
{}

camera& camera::operator=(const camera& cam)
{
	origin = cam.origin;
	lower_left_corner = cam.lower_left_corner;
	horizontal = cam.horizontal;
	vertical = cam.vertical;
	u = cam.u;
	v = cam.v;
	w = cam.w;
	lens_radius = cam.lens_radius;
	time0 = cam.time0;
	time1 = cam.time1;

	return *this;
}
