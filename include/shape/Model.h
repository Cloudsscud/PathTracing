#pragma once

#include "shape/Triangle.h"
#include "accelate/BoundingBox.h"
#include <vector>
#include <filesystem>

class Model : public Shape {
private:
	std::vector<Triangle> m_triangles;
	BoundingBox m_box{};
private:
	void build();
public:
	Model(const std::vector<Triangle>& triangles) :m_triangles(triangles) {}

	Model(const std::filesystem::path& filename);

	std::optional<HitInfo> intersect(
		const Ray& ray,
		float tmin = 1e-5,
		float tmax = std::numeric_limits<float>::infinity()
	) const override;
};