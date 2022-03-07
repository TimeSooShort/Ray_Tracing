#pragma once

#include "ray.h"
#include "texture.h"
#include "hittable.h"

#include <memory>

struct hit_record;

class material
{
public:
	// 产生散射光线；设置光线衰减值
	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
};

// 漫反射
class lambertian : public material
{
public:
	lambertian(const color& a) : albedo(std::make_shared<solid_color>(a)) {}
	lambertian(std::shared_ptr<texture> a) : albedo(a) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		// 反射光
		auto scatter_direction = rec.normal + random_unit_vector();
		// 当随机单位向量正好与法向量相反时，scatter_direction为0，会造成无穷大和 NaN情况的出现
		if (scatter_direction.near_zero())
		{
			scatter_direction = rec.normal;
		}
		scattered = ray(rec.p, scatter_direction, r_in.time());
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		return true;
	}
public:
	//color albedo; //反射率，体现该物质对光的不同吸收程度
	std::shared_ptr<texture> albedo;
};

// 金属
class metal : public material
{
public:
	metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		// reflected 是单位向量
		Vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		// 球体越大，反射越模糊；所谓模糊就是偏移原反射方向一定角度，越偏越模糊
		scattered = ray(rec.p, reflected+fuzz*random_in_unit_sphere(), r_in.time());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}
public:
	color albedo;
	double fuzz; // 模糊参数,参数最大为单位球体半径（零是不扰动）
};

// 电解质，就是透光物质
class dielectric : public material
{
public:
	dielectric(double index_of_refraction) : ir(index_of_refraction) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		attenuation = color(1.0, 1.0, 1.0); // 折射光不衰减
		// 入射光线与被射入物质间的折射率比值，由空气射入则是1.0 / ir，由物质射入空气则是ir
		double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;
		Vec3 unit_direction = unit_vector(r_in.direction());
		// 入射光线与法线夹角,std::fmin 确保精度问题导致的大于1情况的出现
		double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
		double sin_theta = std::sqrt(1 - cos_theta * cos_theta);

		Vec3 direction;
		// 并非所有入射光线都能折射，当refraction_ratio * sin_theta值大于一时
		// 发生 "total internal reflection" 全内反射现象
		bool cannot_refract = refraction_ratio * sin_theta > 1.0;
		if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
		{
			direction = reflect(unit_direction, rec.normal);
		}
		else
		{
			direction = refract(unit_direction, rec.normal, refraction_ratio);
		}
		scattered = ray(rec.p, direction, r_in.time());
		return true;
	}
public:
	double ir; // 该物质的折射率，如空气时1，玻璃是1.5

private:
	// 真正的玻璃具有随角度变化的反射率——以陡峭的角度观察窗户，它就会变成一面镜子
	// Schlick Approximation 施利克近似，就能产生了我们的全玻璃材质
	double reflectance(double cosine, double ref_idx) const {
		auto r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}
};