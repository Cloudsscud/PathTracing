#include "camera/Camera.h"
#include "shape/Sphere.h"
#include "shape/Model.h"
#include "shape/Plane.h"
#include "shape/Scene.h"
#include "util/RGB.h"
#include "renderer/NormalRenderer.h"
#include <renderer/DebugRenderer.h>
#include <renderer/PathTracingRenderer.h>
#include <material/DiffuseMaterial.h>
#include <material/SpecularMaterial.h>
#include <material/DielectricMaterial.h>
#include <material/GroundMaterial.h>

#include <iostream>

int main() {

	Film film(192*4, 108*4);

	Camera camera{ film, { 10, 1.5f,0 }, { 0, 0, 0 }, 45 };

	//Model model("models/dragon/dragon_871k.obj");

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
	for (int i = -3; i <= 3; ++i) {
		scene.addShape(
			sphere,
			new DielectricMaterial{ 1.f + 0.2f * (i + 3), {1,1,1} },
			{ 0, 0.5, i * 2 },
			{ 0.8,0.8, 0.8 }
		);
	}

	scene.addShape(
		plane,
		new GroundMaterial { RGB(120, 204, 157)},
		{ 0,-0.5, 0 }
	);

	auto* light_material = new DiffuseMaterial{ {1,1,1 } };
	light_material->setEmissive({ 0.95,0.95,1 });
	scene.addShape(
		plane,
		light_material,
		{ 0,10, 0 }
	);

	scene.build();	// 场景的加速结构的构建

	NormalRenderer normal_renderer(camera, scene);
	normal_renderer.render(1, "normal.ppm");

	//// 加速结构测试热力图
	BoundsTestCountRenderer btc{ camera, scene };
	btc.render(1, "BTC.ppm");
	TriangleTestCountRenderer ttc{ camera, scene };
	ttc.render(1, "TTC.ppm");
	
	PathTracingRenderer pt_renderer{camera, scene};
	pt_renderer.render(128, "pt_die_test.ppm");


	return 0;
}