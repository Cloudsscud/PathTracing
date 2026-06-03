#include <accelate/BVH.h>
#include <iostream>

void BVH::recusiveSplit(BVHNode* root, size_t depth) {
	// 沿最长轴 递归分割构建BVH树
	if (root->m_triangles.size() <= 1 || depth == 30) {
		return;
	}

	// 根据包围盒对角线找最长轴
	glm::vec3 diag = root->m_bounds.getDiagonal();
	size_t max_axis = diag.x > diag.y ? (diag.x > diag.z ? 0 : 2) : (diag.y > diag.z ? 1 : 2);	// 0 1 2分别对应x y z

	float mid = root->m_bounds.m_min[max_axis] + diag[max_axis] * 0.5f;
	std::vector<Triangle> child0_tri, child1_tri;
	for (const Triangle& tri : root->m_triangles) {
		if ((tri.m_p0[max_axis] + tri.m_p1[max_axis] + tri.m_p2[max_axis]) < 3.f * mid) {
			// 左小右大
			child0_tri.emplace_back(tri);
		}
		else {
			child1_tri.emplace_back(tri);
		}
	}
	BVHNode* child0 = new BVHNode{};
	BVHNode* child1 = new BVHNode{};
	child0->m_triangles = std::move(child0_tri);
	child1->m_triangles = std::move(child1_tri);

	root->m_child[0] = child0;
	root->m_child[1] = child1;
	root->m_triangles.clear();
	root->m_triangles.shrink_to_fit();
	root->m_child[0]->updateBounds();
	root->m_child[1]->updateBounds();

	recusiveSplit(root->m_child[0], depth + 1);
	recusiveSplit(root->m_child[1], depth + 1);
}


void BVH::build(std::vector<Triangle>&& triangles) {
	m_root = new BVHNode;
	m_root->m_triangles = std::move(triangles);
	m_root->updateBounds();

	recusiveSplit(m_root, 0);
}

std::optional<HitInfo> BVH::intersect(const Ray& ray, float tmin, float tmax) const {
	std::optional<HitInfo> hit_info{};
	recusiveIntersect(m_root, ray, tmin, tmax, hit_info);
	return hit_info;
}

void BVH::recusiveIntersect(BVHNode* node, const Ray& ray, float tmin, float& tmax, std::optional<HitInfo>& closest_hit_info) const {
	if (!node || !node->m_bounds.hasIntersection(ray, tmin, tmax)) {
		 // 先看当前包围盒有没有交点
		return;
	}

	// 再判断是否是叶子节点
	if (node->m_triangles.empty()) {
		// 非叶子节点
		recusiveIntersect(node->m_child[0], ray, tmin, tmax, closest_hit_info);
		recusiveIntersect(node->m_child[1], ray, tmin, tmax, closest_hit_info);
	}
	else {
		// 叶子节点，看所含的三角形
		for (const auto& tri : node->m_triangles) {
			auto hit_info = tri.intersect(ray, tmin, tmax);
			if (hit_info.has_value()) {
				tmax = hit_info->m_hit_t;
				closest_hit_info = hit_info;
			}
		}
	}
}
