#pragma once

#include <glm/glm.hpp>
#include <array>

class RGB Lerp(const RGB& a, const RGB& b, float t);

class RGB {
public:
	int m_r, m_g, m_b;

	RGB(int r, int g, int b) :m_r(r), m_g(g), m_b(b) {}

	inline static RGB generateHeatMapRGB(float t) {
		// 调色盘
		std::array<RGB, 25> color_palette{
			RGB { 68, 1, 84 },
			RGB { 71, 17, 100 },
			RGB { 72, 31, 112 },
			RGB { 71, 45, 123 },
			RGB { 68, 58, 131 },

			RGB { 64, 70, 136 },
			RGB { 59, 82, 139 },
			RGB { 54, 93, 141 },
			RGB { 49, 104, 142 },
			RGB { 44, 114, 142 },

			RGB { 40, 124, 142 },
			RGB { 36, 134, 142 },
			RGB { 33, 144, 140 },
			RGB { 31, 154, 138 },
			RGB { 32, 164, 134 },

			RGB { 39, 173, 129 },
			RGB { 53, 183, 121 },
			RGB { 71, 193, 110 },
			RGB { 93, 200, 99 },
			RGB { 117, 208, 84 },

			RGB { 143, 215, 68 },
			RGB { 170, 220, 50 },
			RGB { 199, 224, 32 },
			RGB { 227, 228, 24 },
			RGB { 253, 231, 37 },
		};

		if (t < 0 || t >= 1) {
			return RGB{ 255, 0, 0 };
		}

		float index_float = t * (color_palette.size() - 1);
		int index = std::floor(index_float);
		return Lerp(color_palette[index], color_palette[index + 1], glm::fract(index_float));
	}

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
		m_r = glm::clamp<int>(glm::pow(color.x, 1.f / 2.2f) * 255, 0, 255);
		m_g = glm::clamp<int>(glm::pow(color.y, 1.f / 2.2f) * 255, 0, 255);
		m_b = glm::clamp<int>(glm::pow(color.z, 1.f / 2.2f) * 255, 0, 255);
	}
};

inline RGB Lerp(const RGB& a, const RGB& b, float t){
	return RGB{
		glm::clamp<int>(a.m_r + (b.m_r - a.m_r) * t, 0, 255),
		glm::clamp<int>(a.m_g + (b.m_g - a.m_g) * t, 0, 255),
		glm::clamp<int>(a.m_b + (b.m_b - a.m_b) * t, 0, 255)
	};
}
