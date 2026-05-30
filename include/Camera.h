#pragma once

#include "Film.h"
#include "Ray.h"

// 规定相机为左手坐标系-lookAt z正方向
class Camera {
public:
	Camera(Film& film, const glm::vec3& pos, const glm::vec3& viewpoint, float fovY);

	// 根据屏幕像素坐标和像素内偏移生成一条世界坐标系下的光线
	Ray generateRay(const glm::ivec2& pixel_coord, const glm::vec2& offset = { 0.5f, 0.5f }) const;

	Film& getFilm() { return m_film; }
	const Film& getFilm() const { return m_film; }
private:
	Film m_film;
	glm::vec3 m_pos;

	glm::mat4 m_camera_from_clip;	// 裁剪空间回退到相机空间
	glm::mat4 m_world_from_camera;	// 相机空间回退到世界空间
};