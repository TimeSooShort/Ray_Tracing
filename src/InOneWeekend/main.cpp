#include "const_var.h"
#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "moving_sphere.h"
#include "bvh_node.h"
#include "texture.h"
#include "rtw_stb_image.h"
#include "filesystemUtil.h"
#include "aarect.h"

#include <iostream>
#include <chrono>

using namespace std::chrono;
using Clock = std::chrono::steady_clock;

enum class SELECT_WHICH_RUN
{
	RANDOM_SCENE, TWO_SPHERES, EARTH_PERLIN_SPHERES, CORNEL_BOX
};

color ray_color(const ray& r, const color& background, const hittable& obj, int depth);
//double hit_sphere(const point3& center, double radius, const ray& r);
hittable_list random_scene();
inline void print_program_spend_time(std::chrono::steady_clock::time_point start);
camera select(const SELECT_WHICH_RUN& s, hittable_list& world, color& background);
inline hittable_list two_spheres();
inline hittable_list earth_perlin_spheres();
inline hittable_list cornell_box();


int main()
{
	auto program_start = Clock::now();
	color background;
	// hit objects
	 hittable_list objs;
	//hittable_list objs = random_scene();
	//auto material_ground = std::make_shared<lambertian>(color(0.8, 0.8, 0.0));
	//auto material_center = std::make_shared<lambertian>(color(0.1, 0.2, 0.5));
	//auto material_left = std::make_shared<dielectric>(1.5);
	//auto material_right = std::make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

	//objs.add(std::make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
	//objs.add(std::make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
	//objs.add(std::make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
	//// 当半径取负会导致球面法线向内，对于电解质物质，可以利用这个特性制造空心玻璃球
	//objs.add(std::make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.45, material_left));
	//objs.add(std::make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

	// camera
	//auto viewport_height = 2.0;
	//auto viewport_width = viewport_height * aspect_ratio;
	//auto focal_length = 1.0; // 焦距
	//
	//auto origin = point3(0, 0, 0);
	//auto horizontal = Vec3(viewport_width, 0, 0);
	//auto vertical = Vec3(0, viewport_height, 0);
	//// 视窗左下角坐标
	//auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - Vec3(0, 0, focal_length);

	//camera cam(point3(-2, 2, 1), point3(0, 0, -1), Vec3(0, 1, 0), 90, aspect_ratio); // 远景
	// 放大; vfov值的改变影响视窗的大小，即看到东西的多少，而最终图片大小未变，效果就相当于放大
	//camera cam(point3(-2, 2, 1), point3(0, 0, -1), Vec3(0, 1, 0), 20, aspect_ratio); 

	/*point3 lookfrom(13, 2, 3);
	point3 lookat(0, 0, 0);
	Vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperture = 0.1;

	camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);*/

	camera cam = select(SELECT_WHICH_RUN::CORNEL_BOX, objs, background);

	// render
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int i = image_height-1; i >= 0; --i)
	{
		std::cerr << "\rScanlines remaining: " << i << ' ' << std::flush;
		for (int j = 0; j < image_width; ++j)
		{
			// u,v在零到一之间，利用向上和向右的的vertical，horizontal向量平衡指向左下角的lower_left_corner向量，
			// 使得射线从视窗左上角开始一行行向下扫描
			//ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
			//color pixel_color = ray_color(r);
			//color pixel_color(double(j) / (image_width - 1), double(i) / (image_height - 1), 0.25);
			//color pixel_color = ray_color(r, objs);

			color pixel_color;
			for (int s = 0; s < samples_per_pixel; s++)
			{
				auto u = (j + random_double()) / (image_width - 1);
				auto v = (i + random_double()) / (image_height - 1);
				pixel_color += ray_color(cam.get_ray(u, v), background, objs, max_rebound);
			}
			write_color(std::cout, pixel_color, samples_per_pixel);
		}
	}

	std::cerr << "\nDone.";
	print_program_spend_time(program_start);
}

color ray_color(const ray& r, const color& background, const hittable& obj, int depth)
{
	hit_record rec;
	if (depth <= 0)
	{
		return color(0, 0, 0);
	}

	if (!obj.hit(r, shadow_acne, infinity, rec))
	{
		return background;
	}
	ray scattered;
	color attenuation;
	if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
	{
		return rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
	}
	return attenuation * ray_color(scattered, background, obj, depth - 1);
}

color ray_color_old(const ray& r, const hittable& obj, int depth)
{
	hit_record rec;

	// 确保反射不会一直持续
	if (depth <= 0)
	{
		// 为什么返回0，因为多次反射color相乘，趋近于0，这里超过次数直接归零
		return color(0, 0, 0);
	}

	if (obj.hit(r, shadow_acne, infinity, rec))
	{
		//return 0.5 * (rec.normal + color(1, 1, 1));
		
		// 交点p沿着法向量来到球心，在沿着求内随即方向向量到达求内目标点 
		// p到该目标点的向量就是漫反射向量，递归看该漫反射射线是否集中其他目标。
		// 
		// 在沿曲面法线的单位球偏移中产生随机点。这相当于在半球上拾取方向，高概率接近法线，低概率以掠射角散射光线
		// 该分布按cos(φ)的三次方进行缩放，其中φ是与法线的角度。这是很有用的，因为到达浅角度的光会扩散到更大的区域，因此对最终颜色的贡献较小。
		//point3 target = rec.p + rec.normal + random_in_unit_sphere();
		// 
		// 真 Lambertian 反射，与上面的区别时对单位球内的随机向量进行了规范化,
		// 得到的target点将在球面上，通过在单位球体的曲面上拾取随机点，沿曲面法线偏移
		// 朗伯分布，它的分布为cosφ。对于接近正态的光线散射，真朗伯函数的概率更高，但分布更均匀
		//point3 target = rec.p + rec.normal + random_unit_vector();

		// 半球形散射, 
		//point3 target = rec.p + rec.normal + random_in_hemisphere(rec.normal);
		// 这里每此反弹将光的能量减半
		//return 0.5 * (ray_color(ray(rec.p, target - rec.p), obj, depth-1));

		ray scattered;
		color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		{
			return attenuation * ray_color_old(scattered, obj, depth-1);
		}
		// 没有反射说明没有光能从该交点返回到摄像机里，就是全黑的
		return color(0, 0, 0);
	}
	// sky color
	Vec3 unit_direction = unit_vector(r.direction());
	auto t = 0.5 * (unit_direction.y() + 1.0);
	return (1 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

camera select(const SELECT_WHICH_RUN& s, hittable_list& world, color& background)
{
	point3 lookfrom;
	point3 lookat;
	auto vfov = 40.0;
	auto aperture = 0.0;

	double aspect_ratio_cur = aspect_ratio;

	switch (s)
	{
	case SELECT_WHICH_RUN::RANDOM_SCENE:
		world = random_scene();
		background = color(0.70, 0.80, 1.00);
		lookfrom = point3(13, 2, 3);
		lookat = point3(0, 0, 0);
		vfov = 20.0;
		aperture = 0.1;
		break;
	case SELECT_WHICH_RUN::TWO_SPHERES:
		world = two_spheres();
		background = color(0.70, 0.80, 1.00);
		lookfrom = point3(13, 2, 3);
		lookat = point3(0, 0, 0);
		vfov = 20.0;
		break;
	case SELECT_WHICH_RUN::EARTH_PERLIN_SPHERES:
		world = earth_perlin_spheres();
		background = color(0.70, 0.80, 1.00);
		lookfrom = point3(13, 2, 3);
		lookat = point3(0, 0, 0);
		vfov = 35.0;
		break;
	case SELECT_WHICH_RUN::CORNEL_BOX:
		world = cornell_box();
		background = color(0, 0, 0);
		aspect_ratio_cur = 1.0;
		lookfrom = point3(278, 278, -800);
		lookat = point3(278, 278, 0);
		vfov = 40.0;
		break;
	default:
		break;
	}

	Vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	camera cam(lookfrom, lookat, vup, vfov, aspect_ratio_cur, aperture, dist_to_focus, 0.0, 1.0);
	return cam;
}

inline hittable_list cornell_box()
{
	hittable_list objs;

	auto red = std::make_shared<lambertian>(color(.65, .05, .05));
	auto white = std::make_shared<lambertian>(color(.73, .73, .73));
	auto green = std::make_shared<lambertian>(color(.12, .45, .15));
	auto light = std::make_shared<diffuse_light>(color(15, 15, 15));

	objs.add(std::make_shared<yz_rect>(0, 555, 0, 555, 555, green));
	objs.add(std::make_shared<yz_rect>(0, 555, 0, 555, 0, red));
	objs.add(std::make_shared<xz_rect>(213, 343, 227, 332, 554, light));
	objs.add(std::make_shared<xz_rect>(0, 555, 0, 555, 0, white));
	objs.add(std::make_shared<xz_rect>(0, 555, 0, 555, 555, white));
	objs.add(std::make_shared<xy_rect>(0, 555, 0, 555, 555, white));

	return objs;
}

inline hittable_list two_spheres()
{
	hittable_list objs;
	auto checker = std::make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

	objs.add(std::make_shared<sphere>(point3(0, -10, 0), 10, std::make_shared<lambertian>(checker)));
	objs.add(std::make_shared<sphere>(point3(0, 10, 0), 10, std::make_shared<lambertian>(checker)));

	return objs;
}

inline hittable_list earth_perlin_spheres()
{
	hittable_list objs;
	auto earth = std::make_shared<image_texture>(FileSystemUtil::getPath("/earthmap.jpg").c_str());
	auto noise = std::make_shared<marble_texture>();

	objs.add(std::make_shared<sphere>(point3(0, -1000, 0), 1000, std::make_shared<lambertian>(noise)));
	objs.add(std::make_shared<sphere>(point3(0, 2, 5), 2, std::make_shared<lambertian>(noise)));
	objs.add(std::make_shared<sphere>(point3(0, 2, 0), 2, std::make_shared<lambertian>(earth)));

	return objs;
}

inline hittable_list random_scene()
{
	hittable_list world;
	auto checker = std::make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
	world.add(std::make_shared<sphere>(point3(0, -1000, 0), 1000, std::make_shared<lambertian>(checker)));

	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			auto choose_mat = random_double();
			point3 center(a + 0.8 * random_double(), 0.2, b + 0.8 * random_double());;

			double x = center.x();
			double z = center.z();

			if (x > -5.2 && x < 5.2 && z > -1.2 && z < 1.2)
			{
				if ((center - point3(0, 0.2, 0)).length() < 1.2)
				{
					continue;
				}
				if (x < 0)
				{
					if ((center - point3(-4, 0.2, 0)).length() < 1.2)
					{
						continue;
					}
				}
				else if ((center - point3(4, 0.2, 0)).length() < 1.2)
				{
					continue;
				}
			}
			std::shared_ptr<material> sphere_material;
			if (choose_mat < 0.7)
			{
				// diffuse
				auto albedo = color::random() * color::random();
				sphere_material = std::make_shared<lambertian>(albedo);
				//world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
				// 球最高往上跳0.5
				auto center2 = center + Vec3(0, random_double(0, 0.5), 0);
				world.add(std::make_shared<moving_sphere>(center, center2, 0.0, 1.0, 0.2, sphere_material));
			}
			else if (choose_mat < 0.95) {
				// metal
				auto albedo = color::random(0.5, 1);
				auto fuzz = random_double(0, 0.5);
				sphere_material = std::make_shared<metal>(albedo, fuzz);
				world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
			}
			else {
				// glass
				sphere_material = std::make_shared<dielectric>(1.5);
				world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
			}
		}
	}
	auto material1 = std::make_shared<dielectric>(1.5);
	world.add(std::make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

	auto material2 = std::make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(std::make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

	auto material3 = std::make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(std::make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

	return hittable_list(std::make_shared<bvh_node>(world.objs, 0.0, 1.0));
}

inline void print_program_spend_time(std::chrono::steady_clock::time_point start)
{
	std::cerr << "\nSPEND TIME: " << duration_cast<milliseconds>(Clock::now() - start).count() << " ms\n";
}

// 废弃，功能移到了hittable抽象里
// ======================================================
// 判断射线是否碰触到圆球，球面坐标满足 (x-Cx)2+(y-Cy)2+(z-Cz)2=r2
// 简化为 ( P - C ) * ( P - C ) =r2，P是球面某点，C是球心
// 其中 P 用射线公式表示为 P(t) = A + tb
// ( A + t b - C ) * ( A + t b - C ) =r2，其中A是射线起点，b是射线方向，C是圆心坐标
// 展开得：t2b*b+2tb*(A-C)+(A-C)*(A-C)+r2=0，解二元一次方程组，若是接触则 b方减4ac大于等于0
// center 圆心坐标， radius 半径， r 射线
double hit_sphere(const point3& center, double radius, const ray& r)
{
	Vec3 oc = r.origin() - center; // A-C
	//auto a = dot(r.direction(), r.direction()); // b*b
	auto a = r.direction().length_squared();
	//auto b = 2.0 * dot(oc, r.direction()); // 2b*(A-C)
	auto half_b = dot(oc, r.direction());
	//auto c = dot(oc, oc) - radius * radius; // (A-C)*(A-C)+r2
	auto c = oc.length_squared() - radius * radius;
	//auto discriminant = b * b - 4 * a * c;
	auto discriminant = half_b * half_b - a * c;
	if (discriminant < 0)
	{
		return -1.0;
	}
	else
	{
		return (-half_b - sqrt(discriminant)) / a; // 返回一个方程解
	}
}

color ray_color(const ray& r)
{
	point3 center_point = point3(0, 0, -1);
	auto t = hit_sphere(center_point, 0.5, r);
	if (t > 0.0)
	{
		Vec3 normal = unit_vector(r.at(t) - center_point); // 圆心指向碰触点，即碰触点的法线向量
		return 0.5 * color(normal.x() + 1, normal.y() + 1, normal.z() + 1);
	}
	Vec3 unit_direction = unit_vector(r.direction());
	t = 0.5 * (unit_direction.y() + 1.0); // 单位坐标的范围是[-1,1]加一是为了确保t在[0,1]间
	// 线性插值函数 blendedValue = (1-t)*startValue + t*endValue; 0 <= t <= 1
	return (1 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}