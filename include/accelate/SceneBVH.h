#pragma once
#include <accelate/BoundingBox.h>
#include <shape/Shape.h>

struct ShapeInstance {
	const Shape& m_shape;	// object space 相交的物体
	const Material* m_material;	// 相交实例的材质信息，多态调用
	glm::mat4 m_world_from_object;
	glm::mat4 m_object_from_world;

	BoundingBox m_bounds{};	// world space 的包围盒
	glm::vec3 m_center{};	// world space 包围盒中心点

	// 将包围盒从object space 到 world space
	void updateBounds() {
		auto bounds_object = m_shape.getBounds();
		for (size_t i = 0; i < 8; ++i) {
			auto corner_object = bounds_object.getCorner(i);
			glm::vec3 corner_world = m_world_from_object * glm::vec4(corner_object, 1.f);
			m_bounds.expand(corner_world);
		}
		m_center = (m_bounds.m_max + m_bounds.m_min) * 0.5f;
	}
};

// SceneBVH树状节点
struct SceneBVHTreeNode {
	BoundingBox m_box;
	std::vector<ShapeInstance> m_instances;	// 当前节点包围盒内所含的所有三角形
	SceneBVHTreeNode* m_child[2];	// 左右子树节点
	size_t m_depth;	// 节点在SceneBVH树中的深度
	size_t m_split_axis;	// 切分轴

	void updateBounds() {
		for (const auto& instance : m_instances) {
			m_box.expand(instance.m_bounds);
		}
	}
};

// 数组展开 alignas(32)
struct alignas(32) SceneBVHNode {
	BoundingBox m_box;
	union {
		int m_index_child1;	// 右子树节点的下标
		int m_index_instance;	// 不直接存三角形，降低节点大小
	};

	uint16_t m_instance_count; // 从index开始count个三角形连续存储，有助于缓存命中
	uint8_t m_split_axis;	// 切分轴
};

// 测评SceneBVH的指标
struct SceneBVHState {
	size_t total_node_count{}; // 结构中的总节点数
	size_t total_leaf_node_count{}; // 结构中的叶节点数
	size_t max_leaf_node_instance_count{}; // 叶子节点中最多的三角形数
	size_t max_leaf_node_depth{}; // 叶子节点中最大深度

	void addLeafNode(SceneBVHTreeNode* node) {
		total_leaf_node_count++;
		max_leaf_node_instance_count = std::max(max_leaf_node_instance_count, node->m_instances.size());
		max_leaf_node_depth = std::max(max_leaf_node_depth, node->m_depth);
	}
};

class SceneBVHTreeNodeAllocater {
public:
	// 默认开辟一个4k个节点的缓存
	SceneBVHTreeNodeAllocater() :m_ptr(4096) {}
	~SceneBVHTreeNodeAllocater() {
		for (SceneBVHTreeNode* nodes : m_NodesCache) {
			delete nodes;
		}
		m_NodesCache.clear();
	}

	SceneBVHTreeNode* allocate() {
		if (m_ptr == 4096) {
			// 当一块缓存用完，便再开辟一块缓存，可以减少碎片化的内存
			m_NodesCache.push_back(new SceneBVHTreeNode[4096]);
			m_ptr = 0;
		}
		return &(m_NodesCache.back()[m_ptr++]);	// 取最新开辟缓存中的尚未分配的最近节点
	}
private:
	size_t m_ptr; // 当前缓存块的空闲节点指针
	std::vector<SceneBVHTreeNode*> m_NodesCache;
};

class SceneBVH :public Shape{
private:
	std::vector<SceneBVHNode> m_nodes;	// 存放所有节点:中间结点的左子树节点在其后一个，右子树节点从节点成员找
	std::vector<ShapeInstance> m_instances; // 常规物体实例
	std::vector<ShapeInstance> m_infinity_instances;	// 无限大的物体的实例
	SceneBVHTreeNodeAllocater m_allocater;	// 通过节点分配器整块开辟，减少内存碎片
	
private:
	void recusiveSplit(SceneBVHTreeNode* node, SceneBVHState& state);	// 沿最长轴 递归分割构建SceneBVH树，并记录该SceneBVH结构的评价指标

	size_t recusiveFlatten(SceneBVHTreeNode* node);	// 将SceneBVH树展平为数组结构
public:
	void build(std::vector<ShapeInstance>&& instances);	// 为三角形数组构建SceneBVH加速结构的调用接口

	std::optional<HitInfo> intersect(const Ray& ray, float tmin, float tmax) const override;	// SceneBVH加速与光线求交的调用接口
	BoundingBox getBounds() const override { return m_nodes[0].m_box; }	// SceneBVH包围盒为根节点包围盒
};