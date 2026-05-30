#include "ThreadPoll.h"
#include "Film.h"
#include "Camera.h"
#include "Sphere.h"
#include "Model.h"
#include "Plane.h"
#include "Scene.h"
#include <iostream>

int main() {
	ThreadPoll thread_poll;

	//Film film(1920, 1080);
	Film film(512, 512);

	Camera camera{ film, { 0, 0.8, 5 }, { 0, 0.5, 0 }, 90 };

	Plane plane({
		{0, 0, 0},
		{0, 1, 0}
		});
	Model model("models/bunny/bunny_10k.obj");
	Sphere sphere{ {0, 0, 0}, 0.5f };

	Scene scene;
	scene.addShape(&plane, { 0, -0.5, 0});
	scene.addShape(&model, { 0, 0, 0 }, {2,2,2});
	scene.addShape(&sphere, { 1, 2, 3 }, {0.15,0.2,0.3});

	glm::vec3 light_pos{ 2, 4, 3 };

	std::atomic<int> finished_count = 0;

	auto now = std::chrono::high_resolution_clock::now();
	thread_poll.parallelFor(film.getWidth(), film.getHeight(), [&](size_t x, size_t y) {
		auto ray = camera.generateRay({ x, y });
		auto hit_info = scene.intersect(ray);
		if (hit_info.has_value()) {
			auto l = glm::normalize(light_pos - hit_info->m_hit_pos);
			float cosine = glm::max(0.f, glm::dot(l, hit_info->m_hit_normal));

			film.setPixel(x, y, { cosine, cosine, cosine});
			//film.setPixel(x, y, { 1, 1, 1});
		}
		finished_count++;
		if (finished_count % film.getWidth() == 0) {
			std::cout << static_cast<float>(finished_count) / (film.getWidth() * film.getHeight()) << std::endl;;
		}
		});

	//for (size_t i = 0; i < 1920; ++i) {
	//	for (size_t j = 0; j < 1080; ++j) {
	//		film.setPixel(i, j, { 1., 1., 0. });
	//	}
	//}

	thread_poll.wait();
	auto time = std::chrono::high_resolution_clock::now() - now;
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(time);
	std::cout << ms.count() << "ms\n";

	film.save("test.ppm");

	return 0;
}