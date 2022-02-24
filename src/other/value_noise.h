#pragma once

#include "vec3.h"
#include "interpolation.h"

#include <functional>

/*
* ����������
* 1,������������Ƶ�ʹ���ʱ������ͨ�����������������������ֻ����������Ҫ֪��ʲôʱ�򡰹��ߡ���
�������Ĵ�������ÿ��Ԥ�ȶ�������ֵ֮��ľ���������أ�������������������1����λ
2,�������ڼ����ͼ�ε��������У���һ����������ģ�������������ɵ����ֵ������ͨ����֮Ϊlattice��
3,1)������α����������������Ҫ���ԡ���������������ģ���ȴ��ȷ���Եġ�������ͬ�����룬�����Ƿ�����ͬ��ֵ��
2)��������ֵ��ά���Ƕ��٣�noise�������Ƿ���һ������ֵ; 3)������band limit��ס������Ҫ��һ��������
����԰�������һ���źţ�����㻭��������������õ�һ�����ߣ����������źţ�,���������������ɶ��Ƶ����ɵģ���Ƶ��ζ�Ŵ�Χ�ı仯����Ƶ��ζ��΢С�ı仯����
*/

enum class ValueNaiseType
{
	WHITE_NOISE,VALUE_NOISE,FRACTAL_PATTERN,TURBULENCE_PATTERN,MARBLE_PATTERN,WOOD_PATTERN
};

inline double smoothstep(const double& t)
{
	//t* t* (3 - 2 * t);
	//cos�棺(1 - cos(t * M_PI)) * 0.5; 
	// Perlin : 6t5-15t4+10t3
	return t * t * t * (t * (t * 6 - 15) + 10);
}

class ValueNoise
{
public:
	ValueNoise();

	double eval(Vec3& p) const;

	// ������������:������С�㹻��ķ�Χʱ���������÷ǳ�С���������ظ��������ɼ�;
	// �������ҵ�һ���������ڵĴ�С���ô�С�㹻�����Ա�����Сʱ�޷�ʶ��ģʽ�����㹻�̣�
	// ʹ���ֵ���鱣���൱С�����ֵ��ڴ�ʹ�����ģ���256��512�ƺ��Ǵﵽ�������ĺ�ֵ��
	static const unsigned kMaxTableSize = 256;
	// ���ڳ�����Χ��ֵȡģ��ȷ����ӳ�䵽r������
	static const unsigned kMaxTableSizeMask = kMaxTableSize - 1;
	// �洢����Ķ���ֵ
	double r[kMaxTableSize];
	// ��ϣ��
	// ����ϣ���������������ڴ�Ч�ʣ����ǽ������Լ�ʹ��256�����ֵ�������������������������Ǵ������һά����ά������ά�汾�ĺ���
	// Ϊ�˽��������ƣ�������Ҫ���ľ���ȷ�������������κ�����ֵ�����㡢��ά����ά�㣩������ӳ�䵽���ֵ�����е�һ���ҽ�һ��ֵ
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

	// grid �ĸ������ֵ
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
		// ������
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
		double frequency = 0.05; // Ƶ��
		for (unsigned r = 0; r < imageHeight; ++r)
		{
			for (unsigned c = 0; c < imageWidth; ++c)
			{
				noiseMap[r * imageWidth + c] = noise.eval(Vec3{ c, r, 0 }*frequency);
			}
		}
		break;
	case ValueNaiseType::FRACTAL_PATTERN:
		// ����
		ValueNoise noise;
		double frequency = 0.02; // Ƶ��
		// ͨ���ı��϶�Ⱥ������ֵ���Ի����಻ͬ�����
		double frequencyMult = 1.8; // lacunarity ��϶��
		double amplitudeMult = 0.35; // gain ����
		unsigned numLayers = 5; // ���Ӹ���
		double maxNoiseVal = 0.0; // �����п��ܴ�С����һ����¼���Ĵ�С���ڹ淶��
		for (unsigned r = 0; r < imageHeight; ++r)
		{
			for (unsigned c = 0; c < imageWidth; ++c)
			{
				double amplitude = 1.0; // ���
				Vec3 pNoise = Vec3(c, r, 0.0) * frequency;
				// // ���κͿ����ڴ��������ŷ��ĵ��κ����������Ȼģʽ�����������۵ȣ�
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
		// ����
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
					// ��ʹ������������ֵ��[-1,1]֮�䣬�ٽ�Ϊ���Ĳ���ȡ������������Ч����
					// ������һ���ƺ���͹����ɵ�����,���ɵ����߿�������͹��ƽ
					// ���������ʺ���ģ����̻��Ƶ�ͼ��
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
		// ����ʯ
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
				// ��������ͼ������λ,�Ŷ�������������ģʽ�ĺ���
				// ����˼������������κ������Ի�����Ժ��������������
				noiseMap[r * imageWidth + c] = (sin((c + noiseValue * 100) * 2 * M_PI / 200.0) + 1) / 2.0;
			}
		}
		break;
	case ValueNaiseType::WOOD_PATTERN:
		// ľ������
		ValueNoise noise;
		double frequency = 0.01;
		for (unsigned r = 0; r < imageHeight; ++r)
		{
			for (unsigned c = 0; c < imageWidth; ++c)
			{
				// �����������߻�������Ͽ�(breakups)���������2D����Щbreakups��������ɫ������ɫ����֮��ı߽�
				// �Ӷ�����ľ������
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