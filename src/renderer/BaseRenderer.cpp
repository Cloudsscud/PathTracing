#include "renderer/BaseRenderer.h"
#include "util/Progress.h"
#include "util/Timer.h"
#include "thread/ThreadPoll.h"

void BaseRenderer::render(size_t spp, const std::filesystem::path& filename) {
	TIMER("render " + std::to_string(spp) + " spp for " + filename.string());

	auto& film = m_camera.getFilm();
	film.clear();	// 清理上一次渲染的数据
	Progress progress(film.getHeight() * film.getWidth() * spp, 20);

	size_t current_spp = 0, increase = 1;
	while (current_spp < spp) {
		increase = std::min<size_t>(increase, spp - current_spp);

		// renderer increase
		thread_poll.parallelFor(film.getWidth(), film.getHeight(), [&](size_t x, size_t y) {
			for (int i = 0; i < increase; ++i) {
				film.addSample(x, y, renderPixel({ x, y, current_spp+i }));	// 第三个参数为采样数，确保同一个像素的不同采样点有不同的随机数种子
			}
			progress.update(increase);
		});
		thread_poll.wait();
		current_spp += increase;
		increase = std::min<size_t>(increase*2, 32);	// 一批最大允许32spp
		film.save(filename);
		std::cout << current_spp << "spps for " << filename.string() << std::endl;
	}
}