#pragma once
#include "shape/Shape.h"

struct ShapeInstance {
	const Shape& m_shape;	// object space 相交的物体
	const Material& m_material;	// 相交实例的材质信息
	glm::mat4 m_world_from_object;
	glm::mat4 m_object_from_world;
};

class Scene : public Shape {
public:
	void addShape(const Shape& shape,
		const Material& material,
		const glm::vec3& pos = {0, 0, 0},
		const glm::vec3& scale = {1, 1, 1},
		const glm::vec3& rotate = {0, 0, 0}
	);

	std::optional<HitInfo> intersect(
		const Ray& ray,
		float tmin = 1e-5,
		float tmax = std::numeric_limits<float>::infinity()
	) const override;

private:
	std::vector<ShapeInstance> m_instances;
};