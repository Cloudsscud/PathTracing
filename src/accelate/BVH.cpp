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

	// 基于桶的SAH的优化
	constexpr size_t bucket_count = 12;

	std::vector<size_t> bucket_tri_index[3][bucket_count];	// 存放沿某个轴的第某个桶的三角形在节点的下标
	glm::vec3 diag = node->m_box.getDiagonal();
	std::vector<Triangle> child0_tri, child1_tri;
	// 自行判断最优切分轴，均分十二块来找最小代价的BVH
	float min_cost = std::numeric_limits<float>::infinity();
	size_t min_split_axis = 0;
	size_t min_split_index = 0;
	for (size_t axis = 0; axis < 3; ++axis) {
		size_t bucket_tri_count[bucket_count] = {};	// 存放在这个轴下的某个桶的三角形的数量
		BoundingBox bucket_bound[bucket_count] = {}; // 记录这个轴下的某个桶的三角形的包围盒
		float tmp = 1.f / 3.f;
		size_t tri_index = 0;
		for (const auto& tri : node->m_triangles) {
			float center = (tri.m_p0[axis] + tri.m_p1[axis] + tri.m_p2[axis]) * tmp;
			size_t index_bucket = glm::clamp<size_t>(
				glm::floor((center - node->m_box.m_min[axis]) * bucket_count / diag[axis]),
				0, bucket_count - 1);
			bucket_bound[index_bucket].expand(tri.m_p0);
			bucket_bound[index_bucket].expand(tri.m_p1);
			bucket_bound[index_bucket].expand(tri.m_p2);
			bucket_tri_count[index_bucket]++;
			bucket_tri_index[axis][index_bucket].push_back(tri_index);
			tri_index++;
		}

		BoundingBox left_bound = bucket_bound[0];
		size_t left_tri_count = bucket_tri_count[0];
		for (size_t i = 1; i < bucket_count-1; ++i) {
			BoundingBox right_bound{};
			size_t right_tri_count{};
			for (size_t j = bucket_count-1; j >= i; --j) {
				right_bound.expand(bucket_bound[j]);
				right_tri_count += bucket_tri_count[j];
			}
			if (right_tri_count == 0)
				break;

			if (left_tri_count != 0) {
				float cost = left_bound.getArea() * left_tri_count + right_bound.getArea() * right_tri_count;
				if (cost < min_cost) {
					min_cost = cost;
					min_split_axis = axis;
					node->m_split_axis = axis;
					min_split_index = i;
				}
			}
			left_bound.expand(bucket_bound[i]);
			left_tri_count += bucket_tri_count[i];
		}
	}
	// 找到最小花费的分割方式
	if (min_split_index == 0) {
		// 说明没有分割方式，为叶结点
		state.addLeafNode(node);
		return;
	}
	
	BVHTreeNode* child0 = new BVHTreeNode{};
	BVHTreeNode* child1 = new BVHTreeNode{};
	node->m_child[0] = child0;
	node->m_child[1] = child1;

	child0->m_depth = child1->m_depth = node->m_depth + 1;
	for (size_t bucket_index = 0; bucket_index < min_split_index; ++bucket_index) {
		for (size_t tri_index : bucket_tri_index[min_split_axis][bucket_index]) {
			child0->m_triangles.push_back(node->m_triangles[tri_index]);
		}
	}
	for (size_t bucket_index = min_split_index; bucket_index < bucket_count; ++bucket_index) {
		for (size_t tri_index : bucket_tri_index[min_split_axis][bucket_index]) {
			child1->m_triangles.push_back(node->m_triangles[tri_index]);
		}
	}
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

// 基于SAH的自选轴构建的BVH结构
//Total Node Count : 174195
//Leaf Node Count : 87098
//Triangle Count : 87130
//Mean Leaf Node Triangle Count : 1.00037
//Max Leaf Node Triangle Count : 3
//load model models / dragon / dragon_87k.obj : 2365ms
//render 128 spp for test.ppm : 3049ms

// 三角形数量增大十倍后 731k
//Total Node Count : 1742419
//Leaf Node Count : 871210
//Triangle Count : 871306
//Mean Leaf Node Triangle Count : 1.00011
//Max Leaf Node Triangle Count : 3
//load model models / dragon / dragon_871k.obj : 36521ms
//render 128 spp for test.ppm : 3729ms			// 渲染速率变化不大

// 基于SAH的桶的预划分优化的BVH结构 731k
//Total Node Count : 1742429
//Leaf Node Count : 871215
//Triangle Count : 871306
//Mean Leaf Node Triangle Count : 1.0001
//Max Leaf Node Triangle Count : 3
//load model models / dragon / dragon_871k.obj : 4687ms		// 明显加快
//render 128 spp for test.ppm : 3113ms