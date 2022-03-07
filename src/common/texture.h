#pragma once

#include "vec3.h"
#include "perlin_noise.h"
#include "stb_image.h"

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

class noise_texture : public texture
{
public:
	noise_texture() = default;
	virtual color value(double u, double v, const point3& p) const override
	{
		const PerlinNoise& noise = PerlinNoise::getInstance();
		return color(1.0, 1.0, 1.0) * noise.eval_random_gradient(p);
	}
};

// 大理石
class marble_texture : public texture
{
public:
	marble_texture() = default;
	virtual color value(double u, double v, const point3& p) const override
	{
		const PerlinNoise& noise = PerlinNoise::getInstance();

		double frequency = 2;
		double amplitude = 1.0;
		double frequencyMult = 2;
		double amplitudeMult = 0.5;
		double noiseValue = 0.0;
		unsigned numLayers = 5;
		Vec3 pNoise = Vec3(p) * frequency;
		for (unsigned l = 0; l < numLayers; ++l)
		{
			noiseValue += noise.eval_random_gradient(pNoise) * amplitude;
			pNoise *= frequencyMult;
			amplitude *= amplitudeMult;
		}
		return color(1.0, 1.0, 1.0) * ((sin((noiseValue * 10) + 4 * p.z()) + 1) / 2.0);
	}
};

class image_texture : public texture
{
public:
	const static int bytes_pre_pixel = 3;

	image_texture() : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

	image_texture(const char* filename)
	{
		auto components_per_pixel = bytes_pre_pixel;
		data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);
		if (!data)
		{
			std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
			width = height = 0;
		}
		bytes_per_scanline = bytes_pre_pixel * width;
	}

	~image_texture()
	{
		if (data)
		{
			stbi_image_free(data);
		}
	}

	virtual color value(double u, double v, const point3& p) const override
	{
		// If we have no texture data, then return solid cyan(青色) as a debugging aid.
		if (!data)
		{
			return color(0, 1, 1);
		}
		u = clamp(u, 0.0, 1.0);
		v = 1.0 - clamp(v, 0.0, 1.0);

		// mapping image index
		auto i = static_cast<int>(u * width);
		auto j = static_cast<int>(v * height);
		if (i >= width) i = width - 1;
		if (j >= height) j = height - 1;

		const double color_scale = 1.0 / 255.0;
		auto pixel = data + j * bytes_per_scanline + i * bytes_pre_pixel;

		return (color(pixel[0], pixel[1], pixel[2]) * color_scale);
	}
private:
	unsigned char* data = nullptr;
	int width{}, height{};
	int bytes_per_scanline{}; // 一行所占字节数
};