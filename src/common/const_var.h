#pragma once

#include <limits>
#include <random>

// Constants

constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double PI = 3.14159265358979323846;
// image variable
constexpr auto aspect_ratio = 16.0 / 9.0; // 16.0 / 9.0
constexpr int image_width = 600; // 400;
constexpr int image_height = static_cast<int>(image_width / aspect_ratio);
constexpr int samples_per_pixel = 200; // 100;
// 漫反射最大反弹次数
constexpr int max_rebound = 50;
// 忽略交点附近小范围内的漫反射
constexpr double shadow_acne = 0.001;
// 若是一个向量所有方向的值都小于该值，则视为0向量
constexpr auto minest_length_dimensions = 1e-8;
//
constexpr double aarect_z_ln = 0.0001;

//  random
static std::random_device dev;
static std::mt19937 generator(dev());

// Utility Functions

inline double degrees_to_radians(double degrees)
{
	return degrees * PI / 180.0;
}

inline double random_double(double min, double max)
{
	std::uniform_real_distribution<double> distribution(min, max);
	return distribution(generator);
}

inline double random_double()
{
	static std::uniform_real_distribution<double> distribution;
	return distribution(generator);
}

inline int random_int()
{
	static std::uniform_int_distribution<int> dist(0, 1);

	return dist(generator);
}

inline int random_int(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);

	return dist(generator);
}

inline double clamp(double x, double min, double max)
{
	if (x < min)
	{
		return min;
	}
	else if (x > max)
	{
		return max;
	}
	else
	{
		return x;
	}
}

// Usings