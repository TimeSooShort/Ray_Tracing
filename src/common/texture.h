#pragma once

#include "vec3.h"

#include <memory>

class texture
{
public:
	// u,v 是纹理坐标，p是射线与物体交点
	virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture
{
public:
	solid_color() {}
	solid_color(color c) : color_value(c) {}

	solid_color(double red, double green, double blue) : color_value(red, green, blue) {}

	virtual color value(double u, double v, const point3& p) const override
	{
		return color_value;
	}
private:
	color color_value;
};

class checker_texture : public texture
{
public:
	checker_texture() {}
	checker_texture(std::shared_ptr<texture> odd_txt, std::shared_ptr<texture> even_txt)
		: odd(odd_txt), even(even_txt) {}
	checker_texture(const color& c1, const color& c2) 
		: odd(std::make_shared<solid_color>(c1)), even(std::make_shared<solid_color>(c2)) {}

	virtual color value(double u, double v, const point3& p) const override
	{
		auto sines = std::sin(10 * p.x()) * std::sin(10 * p.y()) * std::sin(10 * p.z());
		if (sines < 0)
		{
			return odd->value(u, v, p);
		}
		else
		{
			return even->value(u, v, p);
		}
	}

	// 指向solid_color纹理或者其他纹理
	std::shared_ptr<texture> odd;
	std::shared_ptr<texture> even;
};