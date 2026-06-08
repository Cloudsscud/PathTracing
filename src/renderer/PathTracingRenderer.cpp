#include <renderer/PathTracingRenderer.h>
#include "util/Frame.h"


glm::vec3 PathTracingRenderer::renderPixel(const glm::ivec2& pixel_coord) {
	auto ray = m_camera.generateRay(pixel_coord, { m_rng.uniform(), m_rng.uniform() });

	glm::vec3 beta = { 1,1,1 };
	glm::vec3 L = { 0,0,0 };	// 在film对应坐标感受到的光照,即Lo(p0,w0->1)

	float q = 0.9;	// 俄罗斯轮盘赌的概率
	while (true) {
		auto hit_info = m_scene.intersect(ray);
		if (!hit_info.has_value()) {
			break;	// 采样的光线与场景无交点
		}
		
		if (m_rng.uniform() > q) {
			break;	// 俄罗斯轮盘赌未通过
		}

		L += beta * hit_info->m_material->m_emissive;	// 轮盘赌过了就先计算交点的光照值
		// 然后采样新的光线，并更新后续存在的交点的光照权重
		Frame frame(hit_info->m_hit_normal);	// 转到局部坐标系下采样新光线
		glm::vec3 light_direction;
		if (hit_info->m_material->m_is_specular) {
			glm::vec3 view_direction = frame.localFromWorld(-ray.m_direction);
			light_direction = { -view_direction.x, view_direction.y, -view_direction.z };
			//float pdf = delta;
			//float brdf = hit_info->m_material->m_albedo * delta / glm::abs(glm::dot(hit_info->m_hit_normal, glm::vec3(0, 1, 0));
			//beta *= brdf * glm::abs(glm::dot(hit_info->m_hit_normal, glm::vec3(0, 1, 0)) / pdf / q;
			// 局部坐标系下的y分量(恒正)就是与y坐标的夹角余弦，即direction.y = glm::abs(glm::dot(hit_info->m_hit_normal, glm::vec3(0, 1, 0))
			// 计算优化
			beta *= hit_info->m_material->m_albedo / q;
		}
		else {
			// 漫反射进行,拒绝采样
			do {
				light_direction = { m_rng.uniform(),m_rng.uniform(),m_rng.uniform() };	// 局部坐标系
				light_direction = light_direction * 2.f - 1.f;	// -1,1
			} while (glm::length(light_direction) > 1);
			if (light_direction.y < 0) {
				light_direction.y = -light_direction.y;	// 上半球
			}
			//float pdf = 1 / 2 / pi;
			//float brdf = hit_info->m_material->m_albedo / pi;
			//beta *= brdf * glm::abs(glm::dot(hit_info->m_hit_normal, glm::vec3(0, 1, 0)) / pdf / q;
			// 计算优化
			light_direction = glm::normalize(light_direction);
			beta *= hit_info->m_material->m_albedo * light_direction.y * 2.f / q;
		}

		ray.m_origin = hit_info->m_hit_pos;
		ray.m_direction = frame.worldFromLocal(light_direction);
	}

	return L;
}