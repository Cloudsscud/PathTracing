#include "camera/Camera.h"
#include "shape/Sphere.h"
#include "shape/Model.h"
#include "shape/Plane.h"
#include "shape/Scene.h"
#include "util/RGB.h"
#include "renderer/NormalRenderer.h"
#include "renderer/TestRTRenderer.h"
#include <renderer/DebugRenderer.h>

#include <iostream>

int main() {

	//Film film(1920, 1080);
	Film film(192*4, 108*4);

	Camera camera{ film, { -7, 0, 0 }, { 0, 0, 0 }, 45 };

	Plane plane({
		{0, 0, 0},
		{0, 1, 0}
		});

	Model model("models/dragon/dragon_871k.obj");

	Sphere sphere{
		{0, 0, 0},
		1.f
	};

	Scene scene;

	scene.addShape(
		plane,
		Material{ {0.1,0.1,0.1}, false, RGB{120, 104, 157} },
		{ 0, -0.5f, 0 }
	);

	RNG r{123456};
	scene.addShape(
		model,
		Material{ {1,1,1}, true, RGB(241, 191, 79) },
		{ 0, 0, 0 }
	);

	scene.addShape(
		sphere,
		{ {1,1,1}, false, RGB{255, 128, 128} },
		{ 0, 0, 2.5f }
	);

	scene.addShape(
		sphere,
		{ {1,1,1}, false, RGB{128, 128, 255} },
		{ 0, 0, -2.5f }
	);

	scene.addShape(
		sphere,
		{ {1,1,1}, true},
		{ 3,0.5,-2 }
	);

	NormalRenderer normal_renderer(camera, scene);
	normal_renderer.render(1, "normal.ppm");

	//// 加速结构测试热力图
	//BoundsTestCountRenderer btc{ camera, scene };
	//btc.render(1, "BTC.ppm");
	//TriangleTestCountRenderer ttc{ camera, scene };
	//ttc.render(1, "TTC.ppm");
	//BoundsDepthRenderer bd{ camera, scene };
	//bd.render(1, "BD.ppm");


	TestRTRenderer rt_renderer{camera, scene};
	rt_renderer.render(128, "test.ppm");


	return 0;
}