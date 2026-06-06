#pragma once
#include <shape/Shape.h>
#include <accelate/SceneBVH.h>

class Scene : public Shape {
public:
	void addShape(const Shape& shape,
		const Material& material = {},
		const glm::vec3& pos = {0, 0, 0},
		const glm::vec3& scale = {1, 1, 1},
		const glm::vec3& rotate = {0, 0, 0}
	);

	std::optional<HitInfo> intersect(
		const Ray& ray,
		float tmin = 1e-5,
		float tmax = std::numeric_limits<float>::infinity()
	) const override;

	void build() {
		m_bvh.build(std::move(m_instances));
	}
private:
	std::vector<ShapeInstance> m_instances;
	SceneBVH m_bvh;
};