#pragma once

#include "shape/Shape.h"

struct Sphere : public Shape {
	Sphere(glm::vec3 center, float r):m_center(center), m_radius(r) {}

	glm::vec3 m_center;
	float m_radius;

	std::optional<HitInfo> intersect(const Ray& ray, float tmin, float tmax) const override;

	BoundingBox getBounds() const override { return {m_center-m_radius, m_center+m_radius}; }

};