#pragma once
#include <accelate/BoundingBox.h>
#include <shape/triangle.h>

struct BVHNode {
	BoundingBox m_bounds;
	std::vector<Triangle> m_triangles;
	BVHNode* m_child[2];

	void updateBounds() {
		for (const auto& triangle : m_triangles) {
			m_bounds.expand(triangle.m_p0);
			m_bounds.expand(triangle.m_p1);
			m_bounds.expand(triangle.m_p2);
		}
	}
};

class BVH :public Shape{
private:
	BVHNode* m_root;
private:
	void recusiveSplit(BVHNode* root, size_t depth);	// 沿最长轴 递归分割构建BVH树

	void recusiveIntersect(
		BVHNode* node,
		const Ray& ray,	float tmin, float& tmax,
		std::optional<HitInfo>&closest_hit_info
	) const;	// 递归与BVH节点包围盒求交,找最近的交点

public:
	void build(std::vector<Triangle>&& triangles);	// 为三角形数组构建BVH加速结构的调用接口

	std::optional<HitInfo> intersect(const Ray& ray, float tmin, float tmax) const override;	// BVH加速与光线求交的调用接口
};