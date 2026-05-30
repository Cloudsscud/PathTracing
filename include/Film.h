#pragma once

#include<filesystem>
#include<vector>
#include<glm/glm.hpp>

class Film {
public:
	Film(size_t width, size_t height);

	void save(const std::filesystem::path& filename);

	size_t getWidth() const { return m_width; }
	size_t getHeight() const { return m_height; }

	glm::vec3 getPixel(size_t x, size_t y) { return m_pixels[y * m_width + x]; }
	void setPixel(size_t x, size_t y, const glm::vec3& color) { m_pixels[y * m_width + x] = color; }
private:
	size_t m_width, m_height;	// 图片的宽高
	std::vector<glm::vec3> m_pixels;	// 存放物理光强
};