#include "Camera.h"

#include<glm/ext/matrix_clip_space.hpp>
#include<glm/ext/matrix_transform.hpp>

Camera::Camera(Film& film, const glm::vec3& pos, const glm::vec3& viewpoint, float fovY) 
	:m_film(film), m_pos(pos)
{
	m_camera_from_clip = glm::inverse(glm::perspective(
		glm::radians(fovY),
		static_cast<float>(film.getWidth() / static_cast<float>(film.getHeight())),
		1.f, 2.f
	));
	
	m_world_from_camera = glm::inverse(glm::lookAt(pos, viewpoint, { 0, 1, 0 }));
}

// 根据屏幕像素坐标和像素内偏移生成一条世界坐标系下的光线
Ray Camera::generateRay(const glm::ivec2& pixel_coord, const glm::vec2& offset) const {
	glm::vec2 ndc = (glm::vec2(pixel_coord) + offset) / glm::vec2(m_film.getWidth(), m_film.getHeight());	// 屏幕空间y向下
	// ndc y向上
	ndc = 1.f - ndc;
	// [0, 1] --> [-1, 1]
	ndc = 2.f * ndc - 1.f;

	glm::vec4 clip{ ndc, 0.f, 1.f };
	glm::vec3 world = m_world_from_camera * m_camera_from_clip * clip;
	return Ray{
		m_pos,			// 光线从相机发出
		glm::normalize(world - m_pos)	// 光线朝向：相机位置指向屏幕某处在世界空间的点
	};
}