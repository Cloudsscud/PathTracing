#pragma once

#include<glm/glm.hpp>
#include <optional>
#include "Ray.h"

struct HitInfo {
	float m_hit_t;
	glm::vec3 m_hit_pos;
	glm::vec3 m_hit_normal;
};

struct Shape {
	virtual std::optional<HitInfo> intersect(const Ray& ray,float tmin,	float tmax) const = 0;
};

