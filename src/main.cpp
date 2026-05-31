#include "thread/ThreadPoll.h"
#include "camera/Camera.h"
#include "shape/Sphere.h"
#include "shape/Model.h"
#include "shape/Plane.h"
#include "shape/Scene.h"
#include "util/Frame.h"
#include "util/RGB.h"
#include "util/RNG.h"
#include "util/Progress.h"
#include "util/Timer.h"

#include <iostream>

int main() {
	ThreadPoll thread_poll;

	//Film film(1920, 1080);
	Film film(192 * 4, 108 * 4);

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
		{ 0.4f,0.4f,0.4f }
	);

	// 启用计时器
	{
		RNG rng(12345612);
		int SPP = 128;
		Progress progress(film.getWidth() * film.getHeight() * SPP);

		TIMER;
		thread_poll.parallelFor(film.getWidth(), film.getHeight(), [&](size_t x, size_t y) {
			for (int i = 0; i < SPP; ++i) {
				auto ray = camera.generateRay({ x, y }, { rng.uniform(), rng.uniform() });

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
								light_direction = { rng.uniform(),rng.uniform(),rng.uniform() };
								light_direction = light_direction * 2.f - 1.f;
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

			progress.update(SPP);
			});
		thread_poll.wait();
	}

	film.save("test.ppm");

	return 0;
}