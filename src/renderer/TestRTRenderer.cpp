#include "renderer/TestRTRenderer.h"
#include "util/Frame.h"

glm::vec3 TestRTRenderer::renderPixel(const glm::ivec2& pixel_coord) {
	auto ray = m_camera.generateRay(pixel_coord, { m_rng.uniform(), m_rng.uniform() });

	glm::vec3 beta = { 1,1,1 };
	glm::vec3 color = { 0,0,0 };

	int count = 0;
	while (true) {

		auto hit_info = m_scene.intersect(ray);
		count++;
		if (hit_info.has_value() && count < 5) {
			color += beta * hit_info->m_material->m_emissive;
			beta *= hit_info->m_material->m_albedo;

			ray.m_origin = hit_info->m_hit_pos;
			// ray.m_direction
			Frame frame(hit_info->m_hit_normal);
			glm::vec3 light_direction;
			if (hit_info->m_material->m_is_specular) {
				// 镜面反射（局部坐标系下的计算）
				glm::vec3 view_direction = frame.localFromWorld(-ray.m_direction);
				light_direction = { -view_direction.x, view_direction.y, -view_direction.z };
			}
			else {
				// 漫反射，均匀采样
				do {
					light_direction = { m_rng.uniform(),m_rng.uniform(),m_rng.uniform() };
					light_direction = light_direction * 2.f - 1.f;
				} while (glm::length(light_direction) > 1);
				if (light_direction.y < 0) {
					light_direction.y = -light_direction.y;	// 上半球
				}
			}
			ray.m_direction = frame.worldFromLocal(light_direction);
		}
		else {
			break;
		}
	}

	return color;
}