#pragma once

#include <optional>
#include "camera/Ray.h"
#include <accelate/BoundingBox.h>

struct Shape {
	virtual std::optional<HitInfo> intersect(const Ray& ray,float tmin,	float tmax) const = 0;
	virtual BoundingBox getBounds() const { return{}; }	// 对象空间下的包围盒，默认退化防止无限大的包围盒
};

