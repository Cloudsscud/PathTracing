#pragma once

#include "camera/Camera.h"
#include "shape/Scene.h"
#include "util/RNG.h"

#define DEFINE_RENDERER(Name) \
	class Name##Renderer : public BaseRenderer{	\
	public:	\
		Name##Renderer(Camera& camera, const Scene& scene) :BaseRenderer(camera, scene) {}	\
	private:	\
		glm::vec3 renderPixel(const glm::ivec2& pixel_coord) override;	\
	}

class BaseRenderer {
protected:
	Camera& m_camera;
	const Scene& m_scene;
	RNG m_rng{static_cast<size_t>(std::time(0))};
public:
	BaseRenderer(Camera& camera, const Scene& scene) :m_camera(camera), m_scene(scene){}

	void render(size_t spp, const std::filesystem::path& filename);	// 多线程并行化渲染流程,逐渐提高质量
	virtual glm::vec3 renderPixel(const glm::ivec2& pixel_coord) = 0;	// 实际渲染方式
};