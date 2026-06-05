#pragma once
#include <accelate/BoundingBox.h>
#include <shape/triangle.h>

// BVH树状节点
struct BVHTreeNode {
	BoundingBox m_box;
	std::vector<Triangle> m_triangles;	// 当前节点包围盒内所含的所有三角形
	BVHTreeNode* m_child[2];	// 左右子树节点
	size_t m_depth;	// 节点在BVH树中的深度
	size_t m_split_axis;	// 切分轴

	void updateBounds() {
		for (const auto& triangle : m_triangles) {
			m_box.expand(triangle.m_p0);
			m_box.expand(triangle.m_p1);
			m_box.expand(triangle.m_p2);
		}
	}
};

// 数组展开 alignas(32)
struct alignas(32) BVHNode {
	BoundingBox m_box;
	union {
		int m_index_child1;	// 右子树节点的下标
		int m_index_tri;	// 不直接存三角形，降低节点大小
	};

	uint16_t m_tri_count; // 从index开始count个三角形连续存储，有助于缓存命中
	uint8_t m_split_axis;	// 切分轴
};

// 测评BVH的指标
struct BVHState {
	size_t total_node_count{}; // 结构中的总节点数
	size_t total_leaf_node_count{}; // 结构中的叶节点数
	size_t max_leaf_node_triangle_count{}; // 叶子节点中最多的三角形数

	void addLeafNode(BVHTreeNode* node) {
		total_leaf_node_count++;
		max_leaf_node_triangle_count = std::max(max_leaf_node_triangle_count, node->m_triangles.size());
	}
};

class BVHTreeNodeAllocater {
public:
	// 默认开辟一个4k个节点的缓存
	BVHTreeNodeAllocater() :m_ptr(4096) {}
	~BVHTreeNodeAllocater() {
		for (BVHTreeNode* nodes : m_NodesCache) {
			delete nodes;
		}
		m_NodesCache.clear();
	}

	BVHTreeNode* allocate() {
		if (m_ptr == 4096) {
			// 当一块缓存用完，便再开辟一块缓存，可以减少碎片化的内存
			m_NodesCache.push_back(new BVHTreeNode[4096]);
			m_ptr = 0;
		}
		return &(m_NodesCache.back()[m_ptr++]);	// 取最新开辟缓存中的尚未分配的最近节点
	}
private:
	size_t m_ptr; // 当前缓存块的空闲节点指针
	std::vector<BVHTreeNode*> m_NodesCache;
};

class BVH :public Shape{
private:
	std::vector<BVHNode> m_nodes;	// 存放所有节点:中间结点的左子树节点在其后一个，右子树节点从节点成员找
	std::vector<Triangle> m_triangles; // 这种存储缓存友好,降低cache miss
	BVHTreeNodeAllocater m_allocater;	// 通过节点分配器整块开辟，减少内存碎片
private:
	void recusiveSplit(BVHTreeNode* node, BVHState& state);	// 沿最长轴 递归分割构建BVH树，并记录该BVH结构的评价指标

	size_t recusiveFlatten(BVHTreeNode* node);	// 将BVH树展平为数组结构
public:
	void build(std::vector<Triangle>&& triangles);	// 为三角形数组构建BVH加速结构的调用接口

	std::optional<HitInfo> intersect(const Ray& ray, float tmin, float tmax) const override;	// BVH加速与光线求交的调用接口
};