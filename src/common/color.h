#pragma once

#include "vec3.h"
#include "const_var.h"
#include <iostream>

void write_color(std::ostream& out, color pixel_color)
{
	out << static_cast<int>(255.999 * pixel_color.x()) << ' '
		<< static_cast<int>(255.999 * pixel_color.y()) << ' '
		<< static_cast<int>(255.999 * pixel_color.z()) << '\n';
}

// 在一个像素中多点采样，这是在模糊话图像，本质上是过滤图像高频函数保留低频，即低频滤波，
// 锯齿出现本质是采样函数的频率低，降低原图频率，能够减少锯齿出现
void write_color(std::ostream& out, color pixel_color, int samplePerPixel)
{
	// 采样时让多个(sample_per_pixel)射线在一个像素里，即对一个像素多次采样，最后取平均值；
	// 函数可以写成不同频率波的和的形式，多次采用平均就是载用不同频率的波叠加，尽量凑近目标函数
	auto scale = 1.0 / samplePerPixel;
	// gamma-correct，产生浅灰色
	auto r = std::sqrt(pixel_color.x() * scale);
	auto g = std::sqrt(pixel_color.y() * scale);
	auto b = std::sqrt(pixel_color.z() * scale);

	out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}