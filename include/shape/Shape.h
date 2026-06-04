#pragma once

#include <optional>
#include "camera/Ray.h"

struct Shape {
	virtual std::optional<HitInfo> intersect(const Ray& ray,float tmin,	float tmax) const = 0;
};

