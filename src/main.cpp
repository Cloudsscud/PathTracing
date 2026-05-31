#include "camera/Camera.h"
#include "shape/Sphere.h"
#include "shape/Model.h"
#include "shape/Plane.h"
#include "shape/Scene.h"
#include "util/RGB.h"
#include "renderer/NormalRenderer.h"
#include "renderer/TestRTRenderer.h"

#include <iostream>

int main() {

	//Film film(1920, 1080);
	Film film(192*4, 108*4);

	Camera camera{ film, { 3, 1.2, 8 }, { 0, 0, 0 }, 45 };

	Plane plane({
		{0, 0, 0},
		{0, 1, 0}
		});

	Model model("models/bunny/bunny_10k.obj");

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
	//	{ {1,1,1}, true },
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

	//NormalRenderer normal_renderer(camera, scene);
	//normal_renderer.render(1, "normal.ppm");
	//film.clear();

	TestRTRenderer rt_renderer{camera, scene};
	rt_renderer.render(50, "test.ppm");
	return 0;
}