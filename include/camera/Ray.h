#pragma once

#include <glm/glm.hpp>
#include <shape/Material.h>
#include <util/DebugMacro.h>

struct HitInfo {
	float m_hit_t;
	glm::vec3 m_hit_pos;
	glm::vec3 m_hit_normal;
	const Material* m_material;

	DEBUG_LINE(size_t bounds_test_count = 0)
	DEBUG_LINE(size_t triangle_test_count = 0)
	DEBUG_LINE(size_t bounds_depth = 0)
};

// world space
struct Ray {
	glm::vec3 m_origin;
	glm::vec3 m_direction;

	glm::vec3 hit(float t) const {
		return m_origin + t * m_direction;
	}

	Ray objectFromWorld(const glm::mat4& object_from_world) const;
};