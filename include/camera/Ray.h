#pragma once

#include <glm/glm.hpp>

// world space
struct Ray {
	glm::vec3 m_origin;
	glm::vec3 m_direction;

	glm::vec3 hit(float t) const {
		return m_origin + t * m_direction;
	}

	Ray objectFromWorld(const glm::mat4& object_from_world) const;
};