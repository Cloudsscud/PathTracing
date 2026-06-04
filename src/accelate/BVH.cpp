#include <accelate/BVH.h>
#include <array>
#include <util/DebugMacro.h>
#include<iostream>

void BVH::build(std::vector<Triangle>&& triangles) {
	size_t triangle_count = triangles.size();
	auto* root = new BVHTreeNode;
	root->m_triangles = std::move(triangles);
	root->updateBounds();
	root->m_depth = 1;

	BVHState state{};

	recusiveSplit(root, state);	// 创建树状BVH

	// 展示BVH结构的测评指标
	std::cout << "Total Node Count: " << state.total_node_count << std::endl;
	std::cout << "Leaf Node Count: " << state.total_leaf_node_count << std::endl;
	std::cout << "Triangle Count: " << triangle_count << std::endl;
	std::cout << "Mean Leaf Node Triangle Count: " << static_cast<float>(triangle_count) / static_cast<float>(state.total_leaf_node_count) << std::endl;
	std::cout << "Max Leaf Node Triangle Count: " << state.max_leaf_node_triangle_count << std::endl;

	recusiveFlatten(root);	// 树状转线性
}

void BVH::recusiveSplit(BVHTreeNode* node, BVHState& state) {
	state.total_node_count++;

	// 沿最长轴 递归分割构建BVH树
	if (node->m_triangles.size() <= 1 || node->m_depth > 32) {
		// 无法再分，就作为叶子节点
		state.addLeafNode(node);
		return;
	}

	// 根据包围盒对角线找最长轴
	glm::vec3 diag = node->m_box.getDiagonal();
	size_t max_axis = diag.x > diag.y ? (diag.x > diag.z ? 0 : 2) : (diag.y > diag.z ? 1 : 2);	// 0 1 2分别对应x y z
	node->m_split_axis = max_axis;

	std::vector<Triangle> child0_tri, child1_tri;
	// 均分十二块来找最小代价的BVH
	float min_cost = std::numeric_limits<float>::infinity();
	for (size_t i = 0; i < 11; ++i) {
		float mid = node->m_box.m_min[max_axis] + diag[max_axis] * (i + 1.f) / 12.f;	// 计算每种分割下的划分线
		std::vector<Triangle> child0_tri_tmp, child1_tri_tmp;
		BoundingBox child0_box{}, child1_box{};
		// 以该划分线开始划分
		for (const Triangle& tri : node->m_triangles) {
			if ((tri.m_p0[max_axis] + tri.m_p1[max_axis] + tri.m_p2[max_axis]) < 3.f * mid) {
				// 左小右大
				child0_tri_tmp.push_back(tri);
				child0_box.expand(tri.m_p0);
				child0_box.expand(tri.m_p1);
				child0_box.expand(tri.m_p2);
			}
			else {
				child1_tri_tmp.push_back(tri);
				child1_box.expand(tri.m_p0);
				child1_box.expand(tri.m_p1);
				child1_box.expand(tri.m_p2);
			}
		}
		// 任一个为空，则该划分方式下不可再分
		if (child0_tri_tmp.empty() || child1_tri_tmp.empty()) {
			continue;
		}

		// SAH计算划分的花费
		float cost = child0_box.getArea() * child0_tri_tmp.size() + child1_box.getArea() * child1_tri_tmp.size();
		if (cost < min_cost) {
			min_cost = cost;
			child0_tri = std::move(child0_tri_tmp);
			child1_tri = std::move(child1_tri_tmp);
		}
	}

	if (child0_tri.empty() || child1_tri.empty()) {
		state.addLeafNode(node);
		return;
	}

	BVHTreeNode* child0 = new BVHTreeNode{};
	BVHTreeNode* child1 = new BVHTreeNode{};
	child0->m_triangles = std::move(child0_tri);
	child1->m_triangles = std::move(child1_tri);
	child0->m_depth = child1->m_depth = node->m_depth + 1;

	node->m_child[0] = child0;
	node->m_child[1] = child1;
	node->m_triangles.clear();
	node->m_triangles.shrink_to_fit();
	node->m_child[0]->updateBounds();
	node->m_child[1]->updateBounds();

	recusiveSplit(node->m_child[0], state);
	recusiveSplit(node->m_child[1], state);
}


/*
* 树状BVH递归方式加速求交 弃置 转递归
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
		static_cast<uint16_t>(node->m_triangles.size()),
		static_cast<uint8_t>(node->m_split_axis)
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

// 迭代方式加速求交
std::optional<HitInfo> BVH::intersect(const Ray& ray, float tmin, float tmax) const {
	std::optional<HitInfo> closest_hit_info;

	DEBUG_LINE(size_t bounds_test_count = 0, triangle_test_count = 0)

	std::array<size_t, 32> stack;
	auto ptr = stack.begin();

	// 依据光线朝向优先选择比较主观上先相交的包围盒
	glm::bvec3 dir_is_neg{
		ray.m_direction.x < 0,
		ray.m_direction.y < 0,
		ray.m_direction.z < 0
	};

	// 光线方向向量的倒数，减少计算量
	glm::vec3 inv_direction = 1.f / ray.m_direction;

	size_t current = 0;
	while (true) {
		auto& node = m_nodes[current];
		DEBUG_LINE(bounds_test_count++)
		if (!node.m_box.hasIntersection(ray, inv_direction, tmin, tmax)) {
			// 与当前包围盒无交点，则取出栈顶指向的节点
			if (ptr == stack.begin()) break;	// 已经指向栈底，则无交点
			current = *(--ptr);	// 继续去判断栈顶节点
			continue;
		}

		if (node.m_tri_count == 0) {
			// 节点为中间结点，则看左右子结点的包围盒
			//// 前序判断
			//current++;	// 左子树结点在其后
			//*(ptr++) = node.m_index_child1;	// 右子树节点存到栈中
			
			// 根据方向选择优先判断的包围盒
			if (dir_is_neg[node.m_split_axis]) {
				// 指向坐标轴负方向
				*(ptr++) = current + 1;	// 后小的
				current = node.m_index_child1;	// 先大的
			}
			else {
				current++;	// 左子树结点在其后
				*(ptr++) = node.m_index_child1;	// 右子树节点存到栈中
			}
		}
		else {
			// 叶子节点,开始求与三角形的交点
			auto it = m_triangles.begin() + node.m_index_tri;
			DEBUG_LINE(triangle_test_count += node.m_tri_count)
			for (auto i = 0; i < node.m_tri_count;++i) {
				auto hit_info = it->intersect(ray, tmin, tmax);
				++it;
				if (hit_info.has_value()) {
					tmax = hit_info->m_hit_t;
					closest_hit_info = hit_info;
					closest_hit_info->bounds_depth = hit_info->bounds_depth;
				}
			}
			// 看栈内是否还有需要判断但尚未判断的包围盒
			if (ptr == stack.begin())	break;
			// 否则继续判断求交
			current = *(--ptr);
		}
	}
	if (closest_hit_info.has_value()) {
		DEBUG_LINE(closest_hit_info->bounds_test_count = bounds_test_count)
		DEBUG_LINE(closest_hit_info->triangle_test_count = triangle_test_count)
	}

	return closest_hit_info;
}

// 沿着最长轴的一半切分构建的BVH结构：
//Total Node Count : 142643
//Leaf Node Count : 71322
//Triangle Count : 87130
//Mean Leaf Node Triangle Count : 1.22164
//Max Leaf Node Triangle Count : 23
//load model models / dragon / dragon_87k.obj : 146ms

// 基于SAH的最长轴构建的BVH结构
//Total Node Count : 164067
//Leaf Node Count : 82034
//Triangle Count : 87130
//Mean Leaf Node Triangle Count : 1.06212
//Max Leaf Node Triangle Count : 8
//load model models / dragon / dragon_87k.obj : 1016ms