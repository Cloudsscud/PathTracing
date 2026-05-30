#pragma once

#include<glm/glm.hpp>

class Material {
public:
	glm::vec3 m_albedo = {1, 1, 1};	// 反照率
	bool m_is_specular = false;	// 镜面反射标记，默认漫反射
	glm::vec3 m_emissive = { 0, 0, 0 }; // 自发光亮度，确保材质可见
};