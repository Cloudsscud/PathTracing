#pragma once

#include <util/RNG.h>
#include <glm/glm.hpp>

constexpr float PI = 3.1415926;

// 均匀圆盘采样
inline glm::vec2 UniformSampleUnitDisk(const glm::vec2& u) {
	float r = glm::sqrt(u.x);
	float theta = 2 * PI * u.y;
	return { r * glm::cos(theta),r * glm::sin(theta) };
}


// 余弦重要性采样
inline glm::vec3 CosineSampleHemisphere(const glm::vec2& u) {
	float r = glm::sqrt(u.x);
	float phi = 2 * PI* u.y;
	return { r * glm::cos(phi), glm::sqrt(1 - r * r), r * glm::sin(phi) };
}

inline float CosineSampleHemispherePDF(const glm::vec3& direction) {
	return direction.y / PI;	// pdf = cos / pi
}

inline glm::vec3 UniformSampleHemisphere(const RNG& rng) {
	glm::vec3 result;
	do {
		result = { rng.uniform(),rng.uniform(),rng.uniform() };	// 局部坐标系
		result = result * 2.f - 1.f;	// -1,1
	} while (glm::length(result) > 1);
	if (result.y < 0) {
		result.y = -result.y;	// 上半球
	}
	return glm::normalize(result);
}