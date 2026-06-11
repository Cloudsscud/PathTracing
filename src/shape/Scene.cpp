#include "shape/Scene.h"
#include <glm/ext/matrix_transform.hpp>

void Scene::addShape(const Shape& shape,const Material* material ,const glm::vec3& pos, const glm::vec3& scale, const glm::vec3& rotate) {
	glm::mat4 world_from_object =
		glm::translate(glm::mat4(1), pos)*
		glm::rotate(glm::mat4(1), glm::radians(rotate.x), {1,0,0})*
		glm::rotate(glm::mat4(1), glm::radians(rotate.y), {0,1,0})*
		glm::rotate(glm::mat4(1), glm::radians(rotate.z), {0,0,1})*
		glm::scale(glm::mat4(1), scale);
	m_instances.push_back(ShapeInstance{ shape, material, world_from_object, glm::inverse(world_from_object) });
}

std::optional<HitInfo> Scene::intersect(const Ray& ray, float tmin, float tmax) const{
	return m_bvh.intersect(ray, tmin, tmax);
}