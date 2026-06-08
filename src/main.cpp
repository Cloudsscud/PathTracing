#include "camera/Camera.h"
#include "shape/Sphere.h"
#include "shape/Model.h"
#include "shape/Plane.h"
#include "shape/Scene.h"
#include "util/RGB.h"
#include "renderer/NormalRenderer.h"
#include "renderer/TestRTRenderer.h"
#include <renderer/DebugRenderer.h>
#include <renderer/PathTracingRenderer.h>

#include <iostream>

int main() {

	Film film(192*4, 108*4);

	Camera camera{ film, { -12, 5,-12 }, { 0, 0, 0 }, 45 };

	Model model("models/dragon/dragon_871k.obj");

	Sphere sphere{
		{0, 0, 0},
		1.f
	};

	Plane plane({
		{0, 0, 0},
		{0, 1, 0}
	});

	Scene scene;
	RNG r{ 1234 };
	for (size_t i = 0; i < 10000; ++i) {
		glm::vec3 random_pos = {
			r.uniform() * 100 - 50,
			r.uniform() * 2,
			r.uniform() * 100 - 50
		};
		float u = r.uniform();
		if (u < 0.9) {
			scene.addShape(
				model,
				{ RGB(200, 150, 100), r.uniform() > 0.5 },
				random_pos,
				{ 1,1,1 },
				{ r.uniform()*360,r.uniform()*360,r.uniform()*360}
			);
		}
		else if (u < 0.95) {
			scene.addShape(
				sphere,
				{ {r.uniform(),r.uniform(),r.uniform()}, true },
				random_pos,
				{0.4,0.4,0.4}
			);
		}
		else {
			random_pos.y += 6;
			scene.addShape(
				sphere,
				{ {1,1,1}, false, {r.uniform()*5,r.uniform()*5,r.uniform()*5} },
				random_pos
			);
		}
	}

	scene.addShape(
		plane,
		{ RGB(120, 120, 120) },
		{ 0,-0.5, 0 }
	);

	scene.build();	// 场景的加速结构的构建

	NormalRenderer normal_renderer(camera, scene);
	normal_renderer.render(1, "normal.ppm");

	//// 加速结构测试热力图
	BoundsTestCountRenderer btc{ camera, scene };
	btc.render(1, "BTC.ppm");
	TriangleTestCountRenderer ttc{ camera, scene };
	ttc.render(1, "TTC.ppm");


	TestRTRenderer rt_renderer{camera, scene};
	rt_renderer.render(128, "rt_test.ppm");
	
	PathTracingRenderer pt_renderer{camera, scene};
	pt_renderer.render(128, "pt_test.ppm");


	return 0;
}