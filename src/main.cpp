#include "ThreadPoll.h"
#include "Film.h"
#include "Camera.h"
#include "Sphere.h"
#include "Model.h"
#include "Plane.h"
#include "Scene.h"
#include "Frame.h"
#include "RGB.h"
#include <iostream>

// 随机数采样
#include <random>

int main() {
	ThreadPoll thread_poll;

	//Film film(1920, 1080);
	Film film(192*4, 108*4);

	Camera camera{ film, { 3, 1.2, 8 }, { 0, 0, 0 }, 45 };

	Plane plane({
		{0, 0, 0},
		{0, 1, 0}
		});

	//Model model("models/bunny/bunny_10k.obj");

	Sphere sphere{
		{0, 0, 0},
		1.f
	};

	Scene scene;

	scene.addShape(
		plane,
		{},
		{ 0, -0.5f, 0 }
	);

	//scene.addShape(
	//	model,
	//	{},
	//	{ 0, 0, 0 }
	//);

	scene.addShape(
		sphere,
		{ {1,1,1}, false, RGB{255, 128, 128} },
		{ 2.5f, 0, 0 }
	);

	scene.addShape(
		sphere,
		{ {1,1,1}, false, RGB{128, 128, 255} },
		{ -2.5f, 0, 0 }
	);
	
	scene.addShape(
		sphere,
		{ {1,1,1}, true, RGB{128, 128, 255} },
		{ 0, 0, -2 },
		{0.4f,0.4f,0.4f}
	);

	std::atomic<int> finished_count = 0;

	std::mt19937 gen(23451334);
	std::uniform_real_distribution<float> uniform(-1, 1);
	int SPP = 128;

	auto now = std::chrono::high_resolution_clock::now();
	thread_poll.parallelFor(film.getWidth(), film.getHeight(), [&](size_t x, size_t y) {
		for (int i = 0; i < SPP; ++i) {
			auto ray = camera.generateRay({ x, y }, { uniform(gen), uniform(gen) });

			glm::vec3 beta = { 1,1,1 };
			glm::vec3 color = { 0,0,0 };

			int count = 0;
			while (true) {
				auto hit_info = scene.intersect(ray);
				count++;
				if (hit_info.has_value() && count < 5) {
					color += beta * hit_info->m_material->m_emissive;
					beta *= hit_info->m_material->m_albedo;

					ray.m_origin = hit_info->m_hit_pos;
					// ray.m_direction
					Frame frame(hit_info->m_hit_normal);
					glm::vec3 light_direction;
					if (hit_info->m_material->m_is_specular) {
						// 镜面反射（局部坐标系下的计算）
						glm::vec3 view_direction = frame.localFromWorld(-ray.m_direction);
						light_direction = { -view_direction.x, view_direction.y, -view_direction.z };
					}
					else {
						// 漫反射，均匀采样
						do {
							light_direction = { uniform(gen),uniform(gen),uniform(gen) };
						} while (glm::length(light_direction) > 1);
						if (light_direction.y < 0) {
							light_direction.y = -light_direction.y;	// 上半球
						}
					}
					ray.m_direction = frame.worldFromLocal(light_direction);
				}
				else {
					break;
				}
			}

			film.addSample(x, y, color);
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