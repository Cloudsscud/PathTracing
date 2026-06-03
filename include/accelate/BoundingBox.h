#pragma once

#include <glm/glm.hpp>
#include "camera/Ray.h"

// AABB包围盒加速求交
struct BoundingBox {
	glm::vec3 m_min;
	glm::vec3 m_max;	// 包围盒离三个轴均最远的点的世界坐标

	BoundingBox() :m_min(std::numeric_limits<float>::infinity()), m_max(-std::numeric_limits<float>::infinity()) {}
	BoundingBox(const glm::vec3& min, const glm::vec3& max) :m_min(min), m_max(max) {}

	void expand(const glm::vec3& pos);

	bool hasIntersection(const Ray& ray, float tmin, float tmax) const;

	glm::vec3 getDiagonal() const { return m_max - m_min; }

};