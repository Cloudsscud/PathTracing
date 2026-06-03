#pragma once

#include "shape/Triangle.h"
#include "accelate/BVH.h"
#include <vector>
#include <filesystem>

class Model : public Shape {
private:
	BVH m_bvh;

public:
	Model(const std::vector<Triangle>& triangles) {
		auto tri = triangles;
		m_bvh.build(std::move(tri));
	}

	Model(const std::filesystem::path& filename);

	std::optional<HitInfo> intersect(
		const Ray& ray,
		float tmin = 1e-5,
		float tmax = std::numeric_limits<float>::infinity()
	) const override;
};