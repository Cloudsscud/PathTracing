#include "camera/Film.h"
#include<fstream>
#include "util/RGB.h"

Film::Film(size_t width, size_t height)
	:m_width(width), m_height(height)
{
	m_pixels.resize(width * height);
}

void Film::save(const std::filesystem::path& filename) {
	// ppm格式
	// P3 三通道rgb / P6 二进制rgb
	// 1920 1080
	// 255
	// 255 0 0
	// 0 255 255
	std::ofstream file(filename, std::ios::binary);
	file << "P6\n" << m_width << ' ' << m_height << "\n255\n";

	for (size_t y = 0; y < m_height; ++y) {
		for (size_t x = 0; x < m_width; ++x) {
			//const glm::vec3 color = getPixel(x, y);
			//glm::u8vec3 color_i = glm::clamp(color * 255.f, 0.f, 255.f);
			/*file << color_i.x <<color_i.y <<color_i.z ;*/
			//file.write(reinterpret_cast<const char*>(&color_i), 3);
			Pixel pixel = getPixel(x, y);
			RGB rgb(pixel.m_color / static_cast<float>(pixel.m_sample_count));
			/*file << color_i.x <<color_i.y <<color_i.z ;*/
			file << static_cast<uint8_t>(rgb.m_r) << static_cast<uint8_t>(rgb.m_g) <<static_cast<uint8_t>(rgb.m_b);
		}
	}
}