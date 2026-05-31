#include "camera/Film.h"
#include<fstream>
#include "util/RGB.h"
#include "util/Timer.h"
#include "thread/ThreadPoll.h"

Film::Film(size_t width, size_t height)
	:m_width(width), m_height(height)
{
	m_pixels.resize(width * height);
}

void Film::save(const std::filesystem::path& filename) {
	TIMER("save to " + filename.string());
	// ppm格式
	// P3 三通道rgb / P6 二进制rgb
	// 1920 1080
	// 255
	// 255 0 0
	// 0 255 255
	std::ofstream file(filename, std::ios::binary);
	file << "P6\n" << m_width << ' ' << m_height << "\n255\n";

	std::vector<uint8_t> buffer(m_width * m_height * 3);
	thread_poll.parallelFor(m_width, m_height, [&](size_t x, size_t y)->void {
		Pixel pixel = getPixel(x, y);
		RGB rgb(pixel.m_color / static_cast<float>(pixel.m_sample_count));
		size_t idx = (x + y * m_width) * 3;
		buffer[idx + 0] = rgb.m_r;
		buffer[idx + 1] = rgb.m_g;
		buffer[idx + 2] = rgb.m_b;
	}, false);
	thread_poll.wait();
	file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());

	//for (size_t y = 0; y < m_height; ++y) {
	//	for (size_t x = 0; x < m_width; ++x) {
	//		//const glm::vec3 color = getPixel(x, y);
	//		//glm::u8vec3 color_i = glm::clamp(color * 255.f, 0.f, 255.f);
	//		/*file << color_i.x <<color_i.y <<color_i.z ;*/
	//		//file.write(reinterpret_cast<const char*>(&color_i), 3);

	//		/*file << color_i.x <<color_i.y <<color_i.z ;*/
	//		file << static_cast<uint8_t>(rgb.m_r) << static_cast<uint8_t>(rgb.m_g) <<static_cast<uint8_t>(rgb.m_b);
	//	}
	//}
}