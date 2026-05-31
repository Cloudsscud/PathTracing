#include "renderer/NormalRenderer.h"

glm::vec3 NormalRenderer::renderPixel(const glm::ivec2& pixel_coord) {
	auto ray = m_camera.generateRay(pixel_coord);
	auto hit_info = m_scene.intersect(ray);
	if (hit_info.has_value()) {
		return { hit_info->m_hit_normal * 0.5f + 0.5f };
	}
	return {};
}