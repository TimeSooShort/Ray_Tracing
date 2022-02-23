#pragma once

#include "const_var.h"
#include "vec3.h"

class camera
{
public:
	// lookfrom:摄像机位置，lookat：摄像机对准的点，vup 世界坐标中的上方，vfov：竖直方向视角域， aspect_ratio：宽高比
	// aperture: 光圈，focus_dist：聚焦平面(focus plane),该平面上的所有东西都处于完美的聚焦状态
	camera(point3 lookfrom, point3 lookat, Vec3 vup, double vfov, double aspect_ratio, 
		double aperture, double focus_dist, double tm0 = 0.0, double tm1 = 0.0)
	{
		auto theta = degrees_to_radians(vfov);
		auto h = std::tan(theta / 2);
		//auto aspect_ratio = 16.0 / 9.0; // 宽/高 比
		auto viewport_height = 2.0 * h; // 视窗高度
		auto viewport_width = aspect_ratio * viewport_height;
		//auto focal_length = 1.0; // 焦距

		// 右手坐标系
		w = unit_vector(lookfrom-lookat); // 摄像机朝向的反向
		u = unit_vector(cross(vup, w)); // 摄像机右轴
		v = cross(w, u); // 摄像机正上方

		origin = lookfrom;
		horizontal = focus_dist * viewport_width * u;
		vertical = focus_dist * viewport_height * v;
		lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

		lens_radius = aperture / 2;
		time0 = tm0;
		time1 = tm1;
	}

	ray get_ray(double s, double t) const
	{
		//return ray(origin, lower_left_corner+s*horizontal+t*vertical-origin);
		// 在光圈内产生一个随机向量，用来移动原origin点
		Vec3 rd = lens_radius * random_in_unit_disk();
		Vec3 offset = u * rd.x() + v * rd.y();

		return ray(
			origin + offset,
			lower_left_corner + s * horizontal + t * vertical - origin - offset,
			random_double(time0, time1)
		);
	}
public:
	point3 origin; // 摄像机位置
	point3 lower_left_corner; // 视窗左下角坐标，代表的是点不是向量
	Vec3 horizontal; // 水平正向向量，大小等于视窗宽度，用来移动射线
	Vec3 vertical; // 竖直正向向量，大小等于视窗高度，用来移动射线
	Vec3 u, v, w; // 相机坐标系
	double lens_radius; // 光圈
	double time0, time1; // 摄像机快门的开关时间，会在这个时间间隔内随机选取某刻发出射线
};