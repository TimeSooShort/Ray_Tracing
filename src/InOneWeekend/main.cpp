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
	//// ���뾶ȡ���ᵼ�����淨�����ڣ����ڵ�������ʣ����������������������Ĳ�����
	//objs.add(std::make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.45, material_left));
	//objs.add(std::make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

	// camera
	//auto viewport_height = 2.0;
	//auto viewport_width = viewport_height * aspect_ratio;
	//auto focal_length = 1.0; // ����
	//
	//auto origin = point3(0, 0, 0);
	//auto horizontal = Vec3(viewport_width, 0, 0);
	//auto vertical = Vec3(0, viewport_height, 0);
	//// �Ӵ����½�����
	//auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - Vec3(0, 0, focal_length);

	//camera cam(point3(-2, 2, 1), point3(0, 0, -1), Vec3(0, 1, 0), 90, aspect_ratio); // Զ��
	// �Ŵ�; vfovֵ�ĸı�Ӱ���Ӵ��Ĵ�С�������������Ķ��٣�������ͼƬ��Сδ�䣬Ч�����൱�ڷŴ�
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
			// u,v���㵽һ֮�䣬�������Ϻ����ҵĵ�vertical��horizontal����ƽ��ָ�����½ǵ�lower_left_corner������
			// ʹ�����ߴ��Ӵ����Ͻǿ�ʼһ��������ɨ��
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

	// ȷ�����䲻��һֱ����
	if (depth <= 0)
	{
		// Ϊʲô����0����Ϊ��η���color��ˣ�������0�����ﳬ������ֱ�ӹ���
		return color(0, 0, 0);
	}

	if (obj.hit(r, shadow_acne, infinity, rec))
	{
		//return 0.5 * (rec.normal + color(1, 1, 1));
		
		// ����p���ŷ������������ģ������������漴����������������Ŀ��� 
		// p����Ŀ�������������������������ݹ鿴�������������Ƿ�������Ŀ�ꡣ
		// 
		// �������淨�ߵĵ�λ��ƫ���в�������㡣���൱���ڰ�����ʰȡ���򣬸߸��ʽӽ����ߣ��͸����������ɢ�����
		// �÷ֲ���cos(��)�����η��������ţ����Ц����뷨�ߵĽǶȡ����Ǻ����õģ���Ϊ����ǳ�ǶȵĹ����ɢ�������������˶�������ɫ�Ĺ��׽�С��
		//point3 target = rec.p + rec.normal + random_in_unit_sphere();
		// 
		// �� Lambertian ���䣬�����������ʱ�Ե�λ���ڵ�������������˹淶��,
		// �õ���target�㽫�������ϣ�ͨ���ڵ�λ�����������ʰȡ����㣬�����淨��ƫ��
		// �ʲ��ֲ������ķֲ�Ϊcos�ա����ڽӽ���̬�Ĺ���ɢ�䣬���ʲ������ĸ��ʸ��ߣ����ֲ�������
		//point3 target = rec.p + rec.normal + random_unit_vector();

		// ������ɢ��, 
		//point3 target = rec.p + rec.normal + random_in_hemisphere(rec.normal);
		// ����ÿ�˷����������������
		//return 0.5 * (ray_color(ray(rec.p, target - rec.p), obj, depth-1));

		ray scattered;
		color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		{
			return attenuation * ray_color_old(scattered, obj, depth-1);
		}
		// û�з���˵��û�й��ܴӸý��㷵�ص�����������ȫ�ڵ�
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
				// �����������0.5
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

// �����������Ƶ���hittable������
// ======================================================
// �ж������Ƿ�������Բ�������������� (x-Cx)2+(y-Cy)2+(z-Cz)2=r2
// ��Ϊ ( P - C ) * ( P - C ) =r2��P������ĳ�㣬C������
// ���� P �����߹�ʽ��ʾΪ P(t) = A + tb
// ( A + t b - C ) * ( A + t b - C ) =r2������A��������㣬b�����߷���C��Բ������
// չ���ã�t2b*b+2tb*(A-C)+(A-C)*(A-C)+r2=0�����Ԫһ�η����飬���ǽӴ��� b����4ac���ڵ���0
// center Բ�����꣬ radius �뾶�� r ����
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
		return (-half_b - sqrt(discriminant)) / a; // ����һ�����̽�
	}
}

color ray_color(const ray& r)
{
	point3 center_point = point3(0, 0, -1);
	auto t = hit_sphere(center_point, 0.5, r);
	if (t > 0.0)
	{
		Vec3 normal = unit_vector(r.at(t) - center_point); // Բ��ָ�������㣬��������ķ�������
		return 0.5 * color(normal.x() + 1, normal.y() + 1, normal.z() + 1);
	}
	Vec3 unit_direction = unit_vector(r.direction());
	t = 0.5 * (unit_direction.y() + 1.0); // ��λ����ķ�Χ��[-1,1]��һ��Ϊ��ȷ��t��[0,1]��
	// ���Բ�ֵ���� blendedValue = (1-t)*startValue + t*endValue; 0 <= t <= 1
	return (1 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}