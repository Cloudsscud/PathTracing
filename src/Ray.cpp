#include "Ray.h"

Ray Ray::objectFromWorld(const glm::mat4& object_from_world) const {
	glm::vec3 oo = object_from_world * glm::vec4(m_origin, 1.f);
	glm::vec3 od = object_from_world * glm::vec4(m_direction, 0.f);
	return Ray{ oo, od };
}