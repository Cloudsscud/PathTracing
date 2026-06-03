#include <accelate/BVH.h>
#include <array>

void BVH::recusiveSplit(BVHTreeNode* root, size_t depth) {
	// 沿最长轴 递归分割构建BVH树
	if (root->m_triangles.size() <= 1 || depth == 30) {
		return;
	}

	// 根据包围盒对角线找最长轴
	glm::vec3 diag = root->m_box.getDiagonal();
	size_t max_axis = diag.x > diag.y ? (diag.x > diag.z ? 0 : 2) : (diag.y > diag.z ? 1 : 2);	// 0 1 2分别对应x y z

	float mid = root->m_box.m_min[max_axis] + diag[max_axis] * 0.5f;
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

	// 该结点不可再分
	if (child0_tri.empty() || child1_tri.empty())
		return;

	BVHTreeNode* child0 = new BVHTreeNode{};
	BVHTreeNode* child1 = new BVHTreeNode{};
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
	auto* root = new BVHTreeNode;
	root->m_triangles = std::move(triangles);
	root->updateBounds();

	recusiveSplit(root, 0);	// 创建树状BVH
	recusiveFlatten(root);	// 树状转线性
}

/*
* 树状BVH 弃置
* 
std::optional<HitInfo> BVH::intersect(const Ray& ray, float tmin, float tmax) const {
	std::optional<HitInfo> hit_info{};
	recusiveIntersect(m_root, ray, tmin, tmax, hit_info);
	return hit_info;
}

void BVH::recusiveIntersect(BVHTreeNode* node, const Ray& ray, float tmin, float& tmax, std::optional<HitInfo>& closest_hit_info) const {
	if (!node || !node->m_box.hasIntersection(ray, tmin, tmax)) {
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
}*/

size_t BVH::recusiveFlatten(BVHTreeNode* node) {
	// 转为数组实际上就是一次前序遍历
	BVHNode bvh_node = {
		node->m_box,
		0,
		static_cast<int>(node->m_triangles.size())
	};
	auto index = m_nodes.size();
	m_nodes.push_back(bvh_node);
	
	if (bvh_node.m_tri_count == 0) {
		// 中间节点处理
		recusiveFlatten(node->m_child[0]);
		m_nodes[index].m_index_child1 = recusiveFlatten(node->m_child[1]);	// 更新右子树节点下标
	}
	else {
		// 叶结点处理
		m_nodes[index].m_index_tri = m_triangles.size();
		for (const auto& tri : node->m_triangles) {
			m_triangles.push_back(tri);
		}
	}
	return index;
}

std::optional<HitInfo> BVH::intersect(const Ray& ray, float tmin, float tmax) const {
	std::optional<HitInfo> closest_hit_info;

	std::array<size_t, 32> stack;
	auto ptr = stack.begin();

	size_t current = 0;
	while (true) {
		auto& node = m_nodes[current];
		if (!node.m_box.hasIntersection(ray, tmin, tmax)) {
			// 与当前包围盒无交点，则取出栈顶指向的节点
			if (ptr == stack.begin()) break;	// 已经指向栈底，则无交点
			current = *(--ptr);	// 继续去判断栈顶节点
			continue;
		}

		if (node.m_tri_count == 0) {
			// 节点为中间结点，则看左右子结点
			// 前序判断
			current++;	// 左子树结点在其后
			*(ptr++) = node.m_index_child1;	// 右子树节点存到栈中
		}
		else {
			// 叶子节点,开始求交点
			auto it = m_triangles.begin() + node.m_index_tri;
			for (auto i = 0; i < node.m_tri_count;++i) {
				auto hit_info = it->intersect(ray, tmin, tmax);
				++it;
				if (hit_info.has_value()) {
					tmax = hit_info->m_hit_t;
					closest_hit_info = hit_info;
				}
			}
			// 看栈内是否还有需要判断但尚未判断的包围盒
			if (ptr == stack.begin())	break;
			// 否则继续判断求交
			current = *(--ptr);
		}
	}
	return closest_hit_info;
}