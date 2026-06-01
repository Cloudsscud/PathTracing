#pragma once

#include <glm/glm.hpp>
#include "camera/Ray.h"

// AABB包围盒加速求交
struct BoundingBox {
	glm::vec3 m_min;
	glm::vec3 m_max;

	BoundingBox() :m_min(0), m_max(0) {}
	BoundingBox(const glm::vec3& min, const glm::vec3& max) :m_min(min), m_max(max) {}

	void expand(const glm::vec3& pos);

	bool hasIntersection(const Ray& ray, float tmin, float tmax) const;
};