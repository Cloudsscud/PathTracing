#include "accelate/BoundingBox.h"

void BoundingBox::expand(const glm::vec3& pos) {
	m_min = glm::min(pos, m_min);
	m_max = glm::max(pos, m_max);
}

bool BoundingBox::hasIntersection(const Ray& ray, float tmin, float tmax) const{
	glm::vec3 t1 = (m_min - ray.m_origin) / ray.m_direction;
	glm::vec3 t2 = (m_max - ray.m_origin) / ray.m_direction;

	glm::vec3 t_min = glm::min(t1, t2);
	glm::vec3 t_max = glm::max(t1, t2);

	float t_enter = glm::max(tmin, glm::max(t_min.x, glm::max(t_min.y, t_min.z)));
	float t_exit = glm::min(tmax, glm::min(t_max.x, glm::min(t_max.y, t_max.z)));

	return t_enter <= t_exit && t_exit >= tmin;
}

bool BoundingBox::hasIntersection(const Ray& ray, const glm::vec3& inv_direction, float tmin, float tmax) const{
	glm::vec3 t1 = (m_min - ray.m_origin) * inv_direction;
	glm::vec3 t2 = (m_max - ray.m_origin) * inv_direction;

	glm::vec3 t_min = glm::min(t1, t2);
	glm::vec3 t_max = glm::max(t1, t2);

	float t_enter = glm::max(tmin, glm::max(t_min.x, glm::max(t_min.y, t_min.z)));
	float t_exit = glm::min(tmax, glm::min(t_max.x, glm::min(t_max.y, t_max.z)));

	return t_enter <= t_exit && t_exit >= tmin;
}