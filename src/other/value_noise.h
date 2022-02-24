#pragma once

#include "vec3.h"
#include "interpolation.h"

#include <functional>

/*
* 关于噪声：
* 1,当噪声函数的频率过高时，可以通过过滤噪声函数来消除这种混叠。问题是要知道什么时候“过高”。
这个问题的答案与标尺上每个预先定义的随机值之间的距离密切相关：两个连续的随机数相距1个单位
2,噪声（在计算机图形的上下文中）是一个函数，它模糊了网格上生成的随机值（我们通常称之为lattice）
3,1)噪音是伪随机这可能是它的主要特性。它看起来是随机的，但却是确定性的。给定相同的输入，它总是返回相同的值。
2)不管输入值的维数是多少，noise函数总是返回一个浮点值; 3)噪音是band limit记住噪声主要是一个函数，
你可以把它看作一个信号（如果你画出这个函数，你会得到一条曲线，这就是你的信号）,噪声函数可能是由多个频率组成的（低频意味着大范围的变化，高频意味着微小的变化）。
*/

enum class ValueNaiseType
{
	WHITE_NOISE,VALUE_NOISE,FRACTAL_PATTERN,TURBULENCE_PATTERN,MARBLE_PATTERN,WOOD_PATTERN
};

inline double smoothstep(const double& t)
{
	//t* t* (3 - 2 * t);
	//cos版：(1 - cos(t * M_PI)) * 0.5; 
	// Perlin : 6t5-15t4+10t3
	return t * t * t * (t * (t * 6 - 15) + 10);
}

class ValueNoise
{
public:
	ValueNoise();

	double eval(Vec3& p) const;

	// 关于噪声特性:当你缩小足够大的范围时，噪音会变得非常小，以至于重复几乎不可见;
	// 诀窍是找到一个噪声周期的大小，该大小足够长，以便在缩小时无法识别模式，但足够短，
	// 使随机值数组保持相当小（保持低内存使用消耗）。256或512似乎是达到这个结果的好值。
	static const unsigned kMaxTableSize = 256;
	// 对于超过范围的值取模，确保其映射到r数组上
	static const unsigned kMaxTableSizeMask = kMaxTableSize - 1;
	// 存储随机的顶点值
	double r[kMaxTableSize];
	// 哈希表
	// 我们希望噪声函数保持内存效率，我们将限制自己使用256个随机值的数组来计算噪声，无论我们处理的是一维、二维还是三维版本的函数
	// 为了解决这个限制，我们需要做的就是确保噪声函数的任何输入值（浮点、二维或三维点）都可以映射到随机值数组中的一个且仅一个值
	unsigned permutationTable[kMaxTableSize * 2];
};

ValueNoise::ValueNoise()
{
	for (int i = 0; i < kMaxTableSize; ++i)
	{
		r[i] = random_double();
		permutationTable[i] = i;
	}

	for (int i = kMaxTableSize - 1; i >= 0; --i)
	{
		if (i)
		{
			std::swap(permutationTable[i], permutationTable[random_int(0, i - 1)]);
			permutationTable[i + kMaxTableSize] = permutationTable[i];
		}
		else
		{
			permutationTable[kMaxTableSize] = permutationTable[0];
		}
	}
}

double ValueNoise::eval(Vec3& p) const
{
	int xi = std::floor(p.x()); // -3.2 -> -4
	int yi = std::floor(p.y());

	double tx = smoothstep(p.x() - xi);
	double ty = smoothstep(p.y() - yi);

	int rx0 = xi & kMaxTableSizeMask;
	int rx1 = (xi + 1) & kMaxTableSizeMask;
	int ry0 = yi & kMaxTableSizeMask;
	int ry1 = (yi + 1) & kMaxTableSizeMask;

	// grid 四个顶点的值
	const double& c00 = r[permutationTable[permutationTable[rx0] + ry0]];
	const double& c10 = r[permutationTable[permutationTable[rx1] + ry0]];
	const double& c01 = r[permutationTable[permutationTable[rx0] + ry1]];
	const double& c11 = r[permutationTable[permutationTable[rx1] + ry1]];

	return bilinear<Vec3>(tx, ty, c00, c10, c01, c11);
}

void valueNoise(const ValueNaiseType& type)
{
	unsigned imageWidth = 512;
	unsigned imageHeight = 512;
	double* noiseMap = new double[imageWidth * imageHeight];

	switch (type)
	{
	case ValueNaiseType::WHITE_NOISE:
		// 白噪音
		for (unsigned r = 0; r < imageHeight; ++r)
		{
			for (unsigned c = 0; c < imageWidth; ++c)
			{
				noiseMap[r * imageWidth + c] = random_double();
}
		}
		break;
	case ValueNaiseType::VALUE_NOISE:
		// value noise
		ValueNoise noise;
		double frequency = 0.05; // 频率
		for (unsigned r = 0; r < imageHeight; ++r)
		{
			for (unsigned c = 0; c < imageWidth; ++c)
			{
				noiseMap[r * imageWidth + c] = noise.eval(Vec3{ c, r, 0 }*frequency);
			}
		}
		break;
	case ValueNaiseType::FRACTAL_PATTERN:
		// 分形
		ValueNoise noise;
		double frequency = 0.02; // 频率
		// 通过改变间隙度和增益的值可以获得许多不同的外观
		double frequencyMult = 1.8; // lacunarity 间隙度
		double amplitudeMult = 0.35; // gain 增益
		unsigned numLayers = 5; // 叠加个数
		double maxNoiseVal = 0.0; // 叠加中可能大小超过一，记录最大的大小用于规范化
		for (unsigned r = 0; r < imageHeight; ++r)
		{
			for (unsigned c = 0; c < imageWidth; ++c)
			{
				double amplitude = 1.0; // 振幅
				Vec3 pNoise = Vec3(c, r, 0.0) * frequency;
				// // 分形和可用于创建令人信服的地形和许多其他自然模式（海景、景观等）
				for (unsigned l = 0; l < numLayers; ++l)
				{
					noiseMap[r * imageWidth + c] += noise.eval(pNoise) * amplitude;
					pNoise *= frequencyMult;
					amplitude *= amplitudeMult;
				}
				if (noiseMap[r * imageWidth + c] > maxNoiseVal)
				{
					maxNoiseVal = noiseMap[r * imageWidth + c];
				}
			}
		}
		for (unsigned i = 0; i < imageHeight * imageWidth; ++i) noiseMap[i] /= maxNoiseVal;
		break;
	case ValueNaiseType::TURBULENCE_PATTERN:
		// 湍流
		ValueNoise noise;
		double frequency = 0.02; 
		double frequencyMult = 1.8; 
		double amplitudeMult = 0.35;
		unsigned numLayers = 5; 
		double maxNoiseVal = 0.0; 
		for (unsigned r = 0; r < imageHeight; ++r)
		{
			for (unsigned c = 0; c < imageWidth; ++c)
			{
				double amplitude = 1.0;
				Vec3 pNoise = Vec3(c, r, 0.0) * frequency;
				for (unsigned l = 0; l < numLayers; ++l)
				{
					// 先使噪声函数返回值再[-1,1]之间，再将为负的部分取反，这样做的效果是
					// 将创建一个似乎由凸起组成的轮廓,生成的曲线看起来凹凸不平
					// 可以生成适合于模拟火、烟或云的图案
					noiseMap[r * imageWidth + c] += std::fabs(2 * noise.eval(pNoise) - 1) * amplitude;
					pNoise *= frequencyMult;
					amplitude *= amplitudeMult;
				}
				if (noiseMap[r * imageWidth + c] > maxNoiseVal)
				{
					maxNoiseVal = noiseMap[r * imageWidth + c];
				}
			}
		}
		for (unsigned i = 0; i < imageHeight * imageWidth; ++i) noiseMap[i] /= maxNoiseVal;
		break;
	case ValueNaiseType::MARBLE_PATTERN:
		// 大理石
		ValueNoise noise;
		double frequency = 0.02; 
		double frequencyMult = 1.8; 
		double amplitudeMult = 0.35; 
		unsigned numLayers = 5;
		for (unsigned r = 0; r < imageHeight; ++r)
		{
			for (unsigned c = 0; c < imageWidth; ++c)
			{
				double amplitude = 1.0;
				Vec3 pNoise = Vec3(c, r, 0.0) * frequency;
				double noiseValue = 0.0;
				for (unsigned l = 0; l < numLayers; ++l)
				{
					noiseValue += noise.eval(pNoise) * amplitude;
					pNoise *= frequencyMult;
					amplitude *= amplitudeMult;
				}
				// 调制正弦图案的相位,扰动我们用来创建模式的函数
				// 这种思想可以用来在任何周期性或规则性函数中引入随机性
				noiseMap[r * imageWidth + c] = (sin((c + noiseValue * 100) * 2 * M_PI / 200.0) + 1) / 2.0;
			}
		}
		break;
	case ValueNaiseType::WOOD_PATTERN:
		// 木质纹理
		ValueNoise noise;
		double frequency = 0.01;
		for (unsigned r = 0; r < imageHeight; ++r)
		{
			for (unsigned c = 0; c < imageWidth; ++c)
			{
				// 噪声函数曲线会出现诸多断开(breakups)的情况，在2D中这些breakups就是亮颜色和深颜色区域之间的边界
				// 从而产生木质纹理
				double g = noise.eval(Vec3(c, r, 0.0) * frequency) * 10;
				noiseMap[r * imageWidth + c] = g - static_cast<int>(g);
			}
		}
		break;
	}
	
	std::ofstream ofs;
	ofs.open("./noise.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << imageWidth << " " << imageHeight << "\n255\n";
	for (unsigned k = 0; k < imageWidth * imageHeight; ++k) {
		unsigned char n = static_cast<unsigned char>(noiseMap[k] * 255);
		ofs << n << n << n;
	}
	ofs.close();

	delete[] noiseMap;
}