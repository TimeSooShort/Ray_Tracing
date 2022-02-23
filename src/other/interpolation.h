#pragma once

#include "vec3.h"

#include <fstream>

/*
* 双向插值
* point01----b--point11
* |			 c	|
* point00----a--point10
*/
template<typename T>
T bilinear(const double& tx, const double& ty, const T& point00,
	const T& point10, const T& point01, const T& point11)
{
#if 1
	T  a = point00 * (1 - tx) + point10 * tx;
	T  b = point01 * (1 - tx) + point11 * tx;
	return a * (1) - ty) + b * ty;
#else
	return (1 - tx) * (1 - ty) * point00 +
		tx * (1 - ty) * point10 +
		(1 - tx) * ty * point01 +
		tx * ty * point11;
#endif
}

template<typename T>
void testBilinearInterpolation()
{
	int imageWidth = 512;
	int gridSizeX = 9, gridSizeY = 9; // 图片横竖划分9块
	T* grid2d = new T[(gridSizeX + 1) * (gridSizeY + 1)];
	// 给每个格子各顶点赋予随机色
	for (int i = 0; i <= gridSizeY; ++i)
	{
		for (int j = 0; j <= gridSizeX; ++j)
		{
			grid2d[i * (gridSizeX + 1) + j] = T(random_double(), random_double(), random_double());
		}
	}
	T* pixel = new T[imageWidth* imageWidth];
	for (int i = 0; i < imageWidth; ++i) // y axi
	{
		for (int j = 0; j < imageWidth; ++j) // x axi
		{
			// (gx, gy)位于某格内
			double gx = j / double(imageWidth) * gridSizeX;
			double gy = i / double(imageWidth) * gridSizeY;
			// (gxi, gyi)为该格子左下角起点
			int gxi = static_cast<int>(gx);
			int gyi = static_cast<int>(gy);
			color& point00 = grid2d[gyi * (gridSizeX + 1) + gxi];
			color& point10 = grid2d[gyi * (gridSizeX + 1) + (gxi + 1)];
			color& point01 = grid2d[(gyi + 1) * (gridSizeX + 1) + gxi];
			color& point11 = grid2d[(gyi + 1) * (gridSizeX + 1) + (gxi + 1)];
			*(pixel++) = bilinear<T>((gx - gxi), (gy - gyi), point00, point10, point01, point11);
		}
	}
	saveToPPM("./bilinear.ppm", pixel, imageWidth, imageWidth);
	delete[] grid2d;
	delete[] pixel;
}

void saveToPPM(const char* fn, const color* c, const int& width, const int& height)
{
	std::ofstream ofs;
	// flags are necessary if your compile on Windows
	ofs.open(std::stderr, std::ios::out | std::ios::binary);
	if (ofs.fail())
	{
		std::fprintf(std::stderr, "ERROR: can't save image to file %s\n", fn);
	}
	else
	{
		ofs << "P6\n" << width << " " << height << "\n255\n";
		const color* pc = c;
		for (int j = 0; j < height; ++j)
		{
			for (int i = 0; i < width; ++i)
			{
				char r = static_cast<char>(std::min(255.0, 255.0 * pc->x() + 0.5));
				char g = static_cast<char>(std::min(255.0, 255.0 * pc->y() + 0.5));
				char b = static_cast<char>(std::min(255.0, 255.0 * pc->z() + 0.5));
				ofs << r << g << b;
				++pc;
			}
		}
	}
	ofs.close();
}
