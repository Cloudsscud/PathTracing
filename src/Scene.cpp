#include "Scene.h"

#include <glm/ext/matrix_transform.hpp>

void Scene::addShape(const Shape& shape,const Material& material ,const glm::vec3& pos, const glm::vec3& scale, const glm::vec3& rotate) {
	glm::mat4 world_from_object =
		glm::translate(glm::mat4(1), pos)*
		glm::rotate(glm::mat4(1), glm::radians(rotate.x), {1,0,0})*
		glm::rotate(glm::mat4(1), glm::radians(rotate.y), {0,1,0})*
		glm::rotate(glm::mat4(1), glm::radians(rotate.z), {0,0,1})*
		glm::scale(glm::mat4(1), scale);
	m_instances.push_back(ShapeInstance{ shape, material, world_from_object, glm::inverse(world_from_object) });
}

std::optional<HitInfo> Scene::intersect(const Ray& ray, float tmin, float tmax) const{
	std::optional<HitInfo> closest_hit_info {};
	const ShapeInstance* closest_instance = nullptr;

	for (const auto& instance : m_instances) {
		auto ray_object = ray.objectFromWorld(instance.m_object_from_world);	// ray和shape所在空间不一致，需要转换，world_ray<->object_shape
		auto hit_info = instance.m_shape.intersect(ray_object, tmin, tmax);
		if (hit_info.has_value()) {
			closest_hit_info = hit_info;
			tmax = hit_info->m_hit_t;
			closest_instance = &instance;
		}
	}

	if (closest_instance) {
		closest_hit_info->m_hit_pos = closest_instance->m_world_from_object * glm::vec4(closest_hit_info.value().m_hit_pos, 1.f);
		// 法线经过缩放的变换矩阵发生变化 => 原先变换矩阵的逆的转置
		closest_hit_info->m_hit_normal = glm::normalize(glm::vec3(glm::transpose(closest_instance->m_object_from_world) * glm::vec4(closest_hit_info.value().m_hit_normal, 0.f)));
		closest_hit_info->m_material = &closest_instance->m_material;
	}
	return closest_hit_info;
}