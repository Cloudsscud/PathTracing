#include <renderer/PathTracingRenderer.h>
#include "util/Frame.h"
#include <util/RNG.h>

glm::vec3 PathTracingRenderer::renderPixel(const glm::ivec3& pixel_coord) {
	thread_local RNG rng{ static_cast<size_t>(pixel_coord.x * 1000000+pixel_coord.y + pixel_coord.z * 100000) };	// 一个线程一个随机数生成器，减少多线程锁等待时间
	// 同时防止同一个线程的随机数不随机，让种子与坐标关联；同时同像素多采样，种子也要与采样数关联


	auto ray = m_camera.generateRay(pixel_coord, { rng.uniform(), rng.uniform() });

	glm::vec3 beta = { 1,1,1 };
	glm::vec3 L = { 0,0,0 };	// 在film对应坐标感受到的光照,即Lo(p0,w0->1)

	float q = 0.9;	// 俄罗斯轮盘赌的概率
	while (true) {
		auto hit_info = m_scene.intersect(ray);
		if (!hit_info.has_value()) {
			break;	// 采样的光线与场景无交点
		}
		L += beta * hit_info->m_material->m_emissive;	// 光源处要先算

		if (rng.uniform() > q) {
			break;	// 俄罗斯轮盘赌未通过
		}
		beta /= q;

		// 然后采样新的光线，并更新后续存在的交点的光照权重
		Frame frame(hit_info->m_hit_normal);	// 转到局部坐标系下采样新光线
		glm::vec3 light_direction;
		if (hit_info->m_material) {
			glm::vec3 view_direction = frame.localFromWorld(-ray.m_direction);
			light_direction = hit_info->m_material->sampleBSDF(hit_info->m_hit_pos, view_direction, beta, rng);

			//float pdf = delta;
			//float brdf = hit_info->m_material->m_albedo * delta / glm::abs(glm::dot(hit_info->m_hit_normal, glm::vec3(0, 1, 0));
			//beta *= brdf * glm::abs(glm::dot(hit_info->m_hit_normal, glm::vec3(0, 1, 0)) / pdf / q;
			// 局部坐标系下的y分量(恒正)就是与y坐标的夹角余弦，即direction.y = glm::abs(glm::dot(hit_info->m_hit_normal, glm::vec3(0, 1, 0))
			// 计算优化
			//beta *= hit_info->m_material->m_albedo / q;
		}
		else {
			break;	// 修改材质之后根据材质自行计算beta和采样

			// 漫反射进行,余弦重要性采样
			//light_direction = CosineSampleHemisphere({ m_rng.uniform(), m_rng.uniform() });	// 减少白噪点

			//pdf = light_direction.y / pi;
			//brdf = hit_info->m_material->m_albedo / pi;
			//beta *= brdf * light_direction.y / pdf / q;
			//beta *= hit_info->m_material->m_albedo / q;

			// 拒绝采样
			//float pdf = 1 / 2 / pi;
			//float brdf = hit_info->m_material->m_albedo / pi;
			//beta *= brdf * glm::abs(glm::dot(hit_info->m_hit_normal, glm::vec3(0, 1, 0)) / pdf / q;
			// 计算优化
			//beta *= hit_info->m_material->m_albedo * light_direction.y * 2.f / q;
		}


		ray.m_origin = hit_info->m_hit_pos;
		ray.m_direction = frame.worldFromLocal(light_direction);
	}

	return L;
}