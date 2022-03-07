#pragma once

#include "vec3.h"

inline double quintic(const double& t);
template<typename T = double> inline T lerp(const T& lo, const T& hi, const T& t);

class PerlinNoise
{
public:
	const static PerlinNoise& getInstance()
	{
		static PerlinNoise noise;
		return noise;
	}

private:
	PerlinNoise();

public:
	PerlinNoise(const PerlinNoise&) = delete;
	void operator=(const PerlinNoise&) = delete;

	double eval_random_gradient(const Vec3& p) const;
	double eval_predefined_gradient(const Vec3& p) const;

private:
	uint8_t hash(const int& x, const int& y, const int& z) const;

	double gradientDotV(
		uint8_t perm, // a value between 0 and 255 
		double x, double y, double z) const;
public:
	static const unsigned tableSize = 256;
	const unsigned tableSizeMask = tableSize - 1;
	Vec3 gradients[tableSize]; // 梯度
	//unsigned permutationTable[tableSize * 2];

	unsigned permutationTable[tableSize*2]{151,160,137,91,90,15,
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };
};

PerlinNoise::PerlinNoise()
{
	for (int i = 0; i < tableSize; ++i)
	{
		// theta与phi详解链接
		// https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/perlin-noise-part-2/perlin-noise
		double theta = acos(2 * random_double() - 1);
		double phi = 2 * random_double() * PI;

		double x = cos(phi) * sin(theta);
		double z = sin(phi) * sin(theta);
		double y = cos(theta);

		gradients[i] = Vec3(x, y, z);
		//permutationTable[i] = i;
		permutationTable[256 + i] = permutationTable[i];
	}

	/*for (int i = tableSize - 1; i >= 0; --i)
	{
		if (i)
		{
			std::swap(permutationTable[i], permutationTable[random_int(0, i - 1)]);
			permutationTable[i + tableSize] = permutationTable[i];
		}
		else
		{
			permutationTable[tableSize] = permutationTable[0];
		}
	}*/
}

double PerlinNoise::eval_random_gradient(const Vec3& p) const
{
	int xi0 = ((int)std::floor(p.x())) & tableSizeMask;
	int yi0 = ((int)std::floor(p.y())) & tableSizeMask;
	int zi0 = ((int)std::floor(p.z())) & tableSizeMask;

	int xi1 = (xi0 + 1) & tableSizeMask;
	int yi1 = (yi0 + 1) & tableSizeMask;
	int zi1 = (zi0 + 1) & tableSizeMask;

	double tx = p.x() - ((int)std::floor(p.x()));
	double ty = p.y() - ((int)std::floor(p.y()));
	double tz = p.z() - ((int)std::floor(p.z()));

	double u = quintic(tx);
	double v = quintic(ty);
	double w = quintic(tz);

	// 单位立方体的八个角的随机梯度向量
	const Vec3& c000 = gradients[hash(xi0, yi0, zi0)];
	const Vec3& c100 = gradients[hash(xi1, yi0, zi0)];
	const Vec3& c010 = gradients[hash(xi0, yi1, zi0)];
	const Vec3& c110 = gradients[hash(xi1, yi1, zi0)];

	const Vec3& c001 = gradients[hash(xi0, yi0, zi1)];
	const Vec3& c101 = gradients[hash(xi1, yi0, zi1)];
	const Vec3& c011 = gradients[hash(xi0, yi1, zi1)];
	const Vec3& c111 = gradients[hash(xi1, yi1, zi1)];

	// generate vectors going from the grid points to p
	double x0 = tx, x1 = tx - 1;
	double y0 = ty, y1 = ty - 1;
	double z0 = tz, z1 = tz - 1;

	Vec3 p000 = Vec3(x0, y0, z0);
	Vec3 p100 = Vec3(x1, y0, z0);
	Vec3 p010 = Vec3(x0, y1, z0);
	Vec3 p110 = Vec3(x1, y1, z0);

	Vec3 p001 = Vec3(x0, y0, z1);
	Vec3 p101 = Vec3(x1, y0, z1);
	Vec3 p011 = Vec3(x0, y1, z1);
	Vec3 p111 = Vec3(x1, y1, z1);

	// linear interpolation
	double a = lerp(dot(c000, p000), dot(c100, p100), u);
	double b = lerp(dot(c010, p010), dot(c110, p110), u);
	double c = lerp(dot(c001, p001), dot(c101, p101), u);
	double d = lerp(dot(c011, p011), dot(c111, p111), u);

	double e = lerp(a, b, v);
	double f = lerp(c, d, v);

	return lerp(e, f, w); // g 
}

double PerlinNoise::eval_predefined_gradient(const Vec3& p) const
{
	int xi0 = ((int)std::floor(p.x())) & tableSizeMask;
	int yi0 = ((int)std::floor(p.y())) & tableSizeMask;
	int zi0 = ((int)std::floor(p.z())) & tableSizeMask;

	int xi1 = (xi0 + 1) & tableSizeMask;
	int yi1 = (yi0 + 1) & tableSizeMask;
	int zi1 = (zi0 + 1) & tableSizeMask;

	double tx = p.x() - ((int)std::floor(p.x()));
	double ty = p.y() - ((int)std::floor(p.y()));
	double tz = p.z() - ((int)std::floor(p.z()));

	double u = quintic(tx);
	double v = quintic(ty);
	double w = quintic(tz);

	// generate vectors going from the grid points to p
	double x0 = tx, x1 = tx - 1;
	double y0 = ty, y1 = ty - 1;
	double z0 = tz, z1 = tz - 1;

	double a = gradientDotV(hash(xi0, yi0, zi0), x0, y0, z0);
	double b = gradientDotV(hash(xi1, yi0, zi0), x1, y0, z0);
	double c = gradientDotV(hash(xi0, yi1, zi0), x0, y1, z0);
	double d = gradientDotV(hash(xi1, yi1, zi0), x1, y1, z0);
	double e = gradientDotV(hash(xi0, yi0, zi1), x0, y0, z1);
	double f = gradientDotV(hash(xi1, yi0, zi1), x1, y0, z1);
	double g = gradientDotV(hash(xi0, yi1, zi1), x0, y1, z1);
	double h = gradientDotV(hash(xi1, yi1, zi1), x1, y1, z1);

	// 就是三维线性插值
	double k0 = a;
	double k1 = (b - a);
	double k2 = (c - a);
	double k3 = (e - a);
	double k4 = (a + d - b - c);
	double k5 = (a + f - b - e);
	double k6 = (a + g - c - e);
	double k7 = (b + c + e + h - a - d - f - g);

	return k0 + k1 * u + k2 * v + k3 * w + k4 * u * v + k5 * u * w + k6 * v * w + k7 * u * v * w;
}

uint8_t PerlinNoise::hash(const int& x, const int& y, const int& z) const
{
	return permutationTable[permutationTable[permutationTable[x] + y] + z];
}

// 预定义的梯度向量图像更具有凹凸，坑坑洼洼感；随机梯度则更平滑
double PerlinNoise::gradientDotV(
	uint8_t perm, // a value between 0 and 255 
	double x, double y, double z) const
{
	// 为了用位运算符代替模运算符，Perlin建议将12个向量的数组扩展到16个向量，在前12个向量的基础上添加以下4个方向
	// 冗余地添加一些方向不会在纹理中引入任何偏差
	switch (perm & 15) {
	case  0: return  x + y; // (1,1,0) 
	case  1: return -x + y; // (-1,1,0) 
	case  2: return  x - y; // (1,-1,0) 
	case  3: return -x - y; // (-1,-1,0) 
	case  4: return  x + z; // (1,0,1) 
	case  5: return -x + z; // (-1,0,1) 
	case  6: return  x - z; // (1,0,-1) 
	case  7: return -x - z; // (-1,0,-1) 
	case  8: return  y + z; // (0,1,1), 
	case  9: return -y + z; // (0,-1,1), 
	case 10: return  y - z; // (0,1,-1), 
	case 11: return -y - z; // (0,-1,-1) 
	case 12: return  y + x; // (1,1,0) 
	case 13: return -x + y; // (-1,1,0) 
	case 14: return -y + z; // (0,-1,1) 
	case 15: return -y - z; // (0,-1,-1) 
	default: return  x + y;
	}
}

// 二阶导数仍连续，详见
// https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/perlin-noise-part-2/improved-perlin-noise
inline double quintic(const double& t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

template<typename T>
inline T lerp(const T& lo, const T& hi, const T& t)
{
	return lo * (1 - t) + hi * t;
}
