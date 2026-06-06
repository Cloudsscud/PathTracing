#include <renderer/DebugRenderer.h>
#include <util/DebugMacro.h>
#include <util/RGB.h>

glm::vec3 BoundsTestCountRenderer::renderPixel(const glm::ivec2& pixel_coord){
	#ifdef DEBUG_INFO
	Ray ray = m_camera.generateRay(pixel_coord);
	m_scene.intersect(ray);
	return RGB::generateHeatMapRGB(ray.bounds_test_count / 150.f);
	return {};
	#else
	return {};
	#endif
}

glm::vec3 TriangleTestCountRenderer::renderPixel(const glm::ivec2& pixel_coord){
#ifdef DEBUG_INFO
	Ray ray = m_camera.generateRay(pixel_coord);
	m_scene.intersect(ray);
	return RGB::generateHeatMapRGB(ray.triangle_test_count / 7.f);
	return {};
#else
	return {};
#endif
}
