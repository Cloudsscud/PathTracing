#include <accelate/SceneBVH.h>
#include <array>
#include <util/DebugMacro.h>
#include<iostream>

void SceneBVH::build(std::vector<ShapeInstance>&& instances) {
	size_t instance_count = instances.size();
	auto* root = m_allocater.allocate();
	// 对实例进行分类
	auto tmp = std::move(instances);
	for (auto& instance : tmp) {
		if (instance.m_shape.getBounds().isValid()) {
			// 将包围盒信息转到world space
			instance.updateBounds();
			root->m_instances.push_back(instance);
		}
		else {
			m_infinity_instances.push_back(instance);
		}
	}
	root->updateBounds();
	root->m_depth = 1;

	SceneBVHState state{};

	recusiveSplit(root, state);	// 创建树状SceneBVH

	// 展示SceneBVH结构的测评指标
	std::cout << "Total Node Count: " << state.total_node_count << std::endl;
	std::cout << "Leaf Node Count: " << state.total_leaf_node_count << std::endl;
	std::cout << "ShapeInstance Count: " << instance_count << std::endl;
	std::cout << "Mean Leaf Node ShapeInstance Count: " << static_cast<float>(instance_count) / static_cast<float>(state.total_leaf_node_count) << std::endl;
	std::cout << "Max Leaf Node ShapeInstance Count: " << state.max_leaf_node_instance_count << std::endl;
	std::cout << "Max Leaf Node Depth: " << state.max_leaf_node_depth << std::endl;

	// 展平时需要对所有节点存为数组，所有三角形存到数组中，但vector的push_back时会频繁扩容与拷贝，而已经记录过相关总数量，可以直接扩够容量，防止性能消耗
	m_nodes.reserve(state.total_node_count);
	m_instances.reserve(instance_count);
	recusiveFlatten(root);	// 树状转线性
}

void SceneBVH::recusiveSplit(SceneBVHTreeNode* node, SceneBVHState& state) {
	state.total_node_count++;

	// 沿最长轴 递归分割构建SceneBVH树
	if (node->m_instances.size() <= 1 || node->m_depth > 32) {
		// 无法再分，就作为叶子节点
		state.addLeafNode(node);
		return;
	}

	// 基于桶的SAH的优化
	constexpr size_t bucket_count = 12;

	std::vector<size_t> bucket_instance_index[3][bucket_count];	// 存放沿某个轴的第某个桶的三角形在节点的下标
	glm::vec3 diag = node->m_box.getDiagonal();

	// 自行判断最优切分轴，均分十二块来找最小代价的SceneBVH结构
	std::vector<ShapeInstance> child0_instance, child1_instance;	// 会频繁扩容，可以在找最优划分后直接扩够
	BoundingBox min_child0_box{}, min_child1_box{};	// 最优划分的包围盒
	size_t min_child0_instance_count{}, min_child1_instance_count{}; // 最优化分的三角形数量
	float min_cost = std::numeric_limits<float>::infinity();
	size_t min_split_axis = 0;
	size_t min_split_index = 0;
	for (size_t axis = 0; axis < 3; ++axis) {
		size_t bucket_instance_count[bucket_count] = {};	// 存放在这个轴下的某个桶的三角形的数量
		BoundingBox bucket_bound[bucket_count] = {}; // 记录这个轴下的某个桶的三角形的包围盒
		float tmp = 1.f / 3.f;
		size_t instance_index = 0;
		for (const auto& instance : node->m_instances) {
			size_t index_bucket = glm::clamp<size_t>(
				glm::floor((instance.m_center[axis] - node->m_box.m_min[axis]) * bucket_count / diag[axis]),
				0, bucket_count - 1);
			bucket_bound[index_bucket].expand(instance.m_bounds);
			bucket_instance_count[index_bucket]++;
			bucket_instance_index[axis][index_bucket].push_back(instance_index);
			instance_index++;
		}

		BoundingBox left_bound = bucket_bound[0];
		size_t left_instance_count = bucket_instance_count[0];
		for (size_t i = 1; i <= bucket_count-1; ++i) {
			BoundingBox right_bound{};
			size_t right_instance_count{};
			for (size_t j = bucket_count-1; j >= i; --j) {
				right_bound.expand(bucket_bound[j]);
				right_instance_count += bucket_instance_count[j];
			}
			if (right_instance_count == 0)
				break;

			if (left_instance_count != 0) {
				float cost = left_bound.getArea() * left_instance_count + right_bound.getArea() * right_instance_count;
				if (cost < min_cost) {
					min_cost = cost;
					min_split_axis = axis;
					node->m_split_axis = axis;
					min_split_index = i;
					min_child0_box = left_bound;
					min_child1_box = right_bound;
					min_child0_instance_count = left_instance_count;
					min_child1_instance_count = right_instance_count;
				}
			}
			left_bound.expand(bucket_bound[i]);
			left_instance_count += bucket_instance_count[i];
		}
	}
	// 找到最小花费的分割方式
	if (min_split_index == 0) {
		// 说明没有分割方式，为叶结点
		state.addLeafNode(node);
		return;
	}
	
	SceneBVHTreeNode* child0 = m_allocater.allocate();
	SceneBVHTreeNode* child1 = m_allocater.allocate();
	node->m_child[0] = child0;
	node->m_child[1] = child1;

	child0->m_depth = child1->m_depth = node->m_depth + 1;
	child0->m_instances.reserve(min_child0_instance_count);
	child1->m_instances.reserve(min_child1_instance_count);
	for (size_t bucket_index = 0; bucket_index < min_split_index; ++bucket_index) {
		for (size_t instance_index : bucket_instance_index[min_split_axis][bucket_index]) {
			child0->m_instances.push_back(node->m_instances[instance_index]);
		}
	}
	for (size_t bucket_index = min_split_index; bucket_index < bucket_count; ++bucket_index) {
		for (size_t instance_index : bucket_instance_index[min_split_axis][bucket_index]) {
			child1->m_instances.push_back(node->m_instances[instance_index]);
		}
	}
	node->m_instances.clear();
	node->m_instances.shrink_to_fit();
	node->m_child[0]->m_box = min_child0_box;
	node->m_child[1]->m_box = min_child1_box;

	recusiveSplit(node->m_child[0], state);
	recusiveSplit(node->m_child[1], state);
}

size_t SceneBVH::recusiveFlatten(SceneBVHTreeNode* node) {
	// 转为数组实际上就是一次前序遍历
	SceneBVHNode bvh_node = {
		node->m_box,
		0,
		static_cast<uint16_t>(node->m_instances.size()),
		static_cast<uint8_t>(node->m_split_axis)
	};
	auto index = m_nodes.size();
	m_nodes.push_back(bvh_node);
	
	if (bvh_node.m_instance_count == 0) {
		// 中间节点处理
		recusiveFlatten(node->m_child[0]);
		m_nodes[index].m_index_child1 = recusiveFlatten(node->m_child[1]);	// 更新右子树节点下标
	}
	else {
		// 叶结点处理
		m_nodes[index].m_index_instance = m_instances.size();
		for (const auto& instance : node->m_instances) {
			m_instances.push_back(instance);
		}
	}
	return index;
}

// 迭代方式加速求交
std::optional<HitInfo> SceneBVH::intersect(const Ray& ray, float tmin, float tmax) const {
	std::optional<HitInfo> closest_hit_info;
	const ShapeInstance* closest_instance = nullptr;

	DEBUG_LINE(size_t bounds_test_count = 0)

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

		if (node.m_instance_count == 0) {
			// 节点为中间结点，则看左右子结点的包围盒
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
			auto it = m_instances.begin() + node.m_index_instance;
			for (auto i = 0; i < node.m_instance_count;++i) {
				auto ray_object = ray.objectFromWorld(it->m_object_from_world);	// 将光线转换到object space
				auto hit_info = it->m_shape.intersect(ray_object, tmin, tmax);	// 在object space下进行相交测试
				// 修改world的调试信息
				DEBUG_LINE(ray.bounds_test_count += ray_object.bounds_test_count);
				DEBUG_LINE(ray.triangle_test_count += ray_object.triangle_test_count);
				if (hit_info.has_value()) {
					tmax = hit_info->m_hit_t;
					closest_hit_info = hit_info;
					closest_instance = &(*it);	// 获取最近相交的shape实例
				}
				++it;
			}
			// 看栈内是否还有需要判断但尚未判断的包围盒
			if (ptr == stack.begin())	break;
			// 否则继续判断求交
			current = *(--ptr);
		}
	}
	// 再与无限大的物体进行求交
	for (const auto& infinity_instance : m_infinity_instances) {
		auto ray_object = ray.objectFromWorld(infinity_instance.m_object_from_world);	// 将光线转换到object space
		auto hit_info = infinity_instance.m_shape.intersect(ray_object, tmin, tmax);	// 在object space下进行相交测试
		// 修改world的调试信息
		DEBUG_LINE(ray.bounds_test_count += ray_object.bounds_test_count);
		DEBUG_LINE(ray.triangle_test_count += ray_object.triangle_test_count);
		if (hit_info.has_value()) {
			tmax = hit_info->m_hit_t;
			closest_hit_info = hit_info;
			closest_instance = &infinity_instance;	// 获取最近相交的实例
		}
	}

	// 将object space的信息转到world space
	if (closest_instance) {
		closest_hit_info->m_hit_pos = closest_instance->m_world_from_object * glm::vec4(closest_hit_info.value().m_hit_pos, 1.f);
		// 法线经过缩放的变换矩阵发生变化 => 原先变换矩阵的逆的转置
		closest_hit_info->m_hit_normal = glm::normalize(glm::vec3(glm::transpose(closest_instance->m_object_from_world) * glm::vec4(closest_hit_info->m_hit_normal, 0.f)));
		closest_hit_info->m_material = &closest_instance->m_material;
	}

	DEBUG_LINE(ray.bounds_test_count += bounds_test_count)

	return closest_hit_info;
}