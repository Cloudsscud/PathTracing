//#include "renderer/TestRTRenderer.h"
//#include "util/Frame.h"
//#include <sample/Spherical.h>
//
//glm::vec3 TestRTRenderer::renderPixel(const glm::ivec2& pixel_coord) {
//	auto ray = m_camera.generateRay(pixel_coord, { m_rng.uniform(), m_rng.uniform() });
//
//	glm::vec3 beta = { 1,1,1 };
//	glm::vec3 color = { 0,0,0 };
//
//	size_t max_count = 32;
//	while (max_count--) {
//		auto hit_info = m_scene.intersect(ray);
//		if (hit_info.has_value()) {
//			color += beta * hit_info->m_material->m_emissive;
//			beta *= hit_info->m_material->m_albedo;
//
//			ray.m_origin = hit_info->m_hit_pos;
//			// ray.m_direction
//			Frame frame(hit_info->m_hit_normal);
//			glm::vec3 light_direction;
//			if (hit_info->m_material->m_is_specular) {
//				// 镜面反射（局部坐标系下的计算）
//				glm::vec3 view_direction = frame.localFromWorld(-ray.m_direction);
//				light_direction = { -view_direction.x, view_direction.y, -view_direction.z };
//			}
//			else {
//				// 漫反射，拒绝采样
//				light_direction = UniformSampleHemisphere(m_rng);
//			}
//			ray.m_direction = frame.worldFromLocal(light_direction);
//		}
//		else {
//			break;
//		}
//	}
//
//	return color;
//}