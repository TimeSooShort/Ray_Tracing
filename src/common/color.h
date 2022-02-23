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

// ��һ�������ж�������������ģ����ͼ�񣬱������ǹ���ͼ���Ƶ����������Ƶ������Ƶ�˲���
// ��ݳ��ֱ����ǲ���������Ƶ�ʵͣ�����ԭͼƵ�ʣ��ܹ����پ�ݳ���
void write_color(std::ostream& out, color pixel_color, int samplePerPixel)
{
	// ����ʱ�ö��(sample_per_pixel)������һ�����������һ�����ض�β��������ȡƽ��ֵ��
	// ��������д�ɲ�ͬƵ�ʲ��ĺ͵���ʽ����β���ƽ���������ò�ͬƵ�ʵĲ����ӣ������ս�Ŀ�꺯��
	auto scale = 1.0 / samplePerPixel;
	// gamma-correct������ǳ��ɫ
	auto r = std::sqrt(pixel_color.x() * scale);
	auto g = std::sqrt(pixel_color.y() * scale);
	auto b = std::sqrt(pixel_color.z() * scale);

	out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}