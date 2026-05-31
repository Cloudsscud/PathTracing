#pragma once

#include <glm/glm.hpp>

class RGB {
public:
	float m_r, m_g, m_b;
	
	RGB(float r, float g, float b) :m_r(r), m_g(g), m_b(b) {}

	// 允许解码得到光强
	operator glm::vec3() const {
		return glm::vec3{
			glm::pow(m_r / 255.f, 2.2f),
			glm::pow(m_g / 255.f, 2.2f),
			glm::pow(m_b / 255.f, 2.2f)
		};
	}

	// 对光强进行gamma校正，得到sRGB
	RGB(const glm::vec3& color) {
		m_r = glm::clamp(glm::pow(color.x, 1.f / 2.2f) * 255.f, 0.f, 255.f);
		m_g = glm::clamp(glm::pow(color.y, 1.f / 2.2f) * 255.f, 0.f, 255.f);
		m_b = glm::clamp(glm::pow(color.z, 1.f / 2.2f) * 255.f, 0.f, 255.f);
	}
};