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
			Material* material;
			if (r.uniform() > 0.5) {
				material = new DiffuseMaterial{ RGB(200, 150, 100) };
			}
			else {
				material = new SpecularMaterial{ RGB(200, 150, 100) };
			}

			scene.addShape(
				model,
				material,
				random_pos,
				{ 1,1,1 },
				{ r.uniform()*360,r.uniform()*360,r.uniform()*360}
			);
		}
		else if (u < 0.95) {
			scene.addShape(
				sphere,
				new SpecularMaterial{ {r.uniform(),r.uniform(),r.uniform()}},
				random_pos,
				{0.4,0.4,0.4}
			);
		}
		else {
			random_pos.y += 6;
			auto* material = new DiffuseMaterial{ { 0,0,0} };
			material->setEmissive({ r.uniform() * 5,r.uniform() * 5,r.uniform() * 5 });
			scene.addShape(
				sphere,
				material,
				random_pos
			);
		}
	}

	scene.addShape(
		plane,
		new DiffuseMaterial{ RGB(120, 120, 120)},
		{ 0,-0.5, 0 }
	);

	scene.build();	// 场景的加速结构的构建

	//NormalRenderer normal_renderer(camera, scene);
	//normal_renderer.render(1, "normal.ppm");

	//// 加速结构测试热力图
	BoundsTestCountRenderer btc{ camera, scene };
	btc.render(1, "BTC.ppm");
	TriangleTestCountRenderer ttc{ camera, scene };
	ttc.render(1, "TTC.ppm");
	
	PathTracingRenderer pt_renderer{camera, scene};
	pt_renderer.render(128, "pt_cos_test.ppm");


	return 0;
}