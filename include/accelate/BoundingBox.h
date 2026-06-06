#pragma once

#include <glm/glm.hpp>
#include "camera/Ray.h"

// AABB包围盒加速求交
struct BoundingBox {
	glm::vec3 m_min;
	glm::vec3 m_max;	// 包围盒离三个轴均最远的点的世界坐标

	BoundingBox() :m_min(std::numeric_limits<float>::infinity()), m_max(-std::numeric_limits<float>::infinity()) {}
	BoundingBox(const glm::vec3& min, const glm::vec3& max) :m_min(min), m_max(max) {}

	void expand(const glm::vec3& pos);
	void expand(const BoundingBox& bound);

	bool hasIntersection(const Ray& ray, float tmin, float tmax) const;
	bool hasIntersection(const Ray& ray,const glm::vec3& inv_direction , float tmin, float tmax) const;

	glm::vec3 getDiagonal() const { return m_max - m_min; }
	float getArea() const {
		glm::vec3 diag = getDiagonal();
		return (diag.x * (diag.y + diag.z) + diag.y * diag.z) * 2;
	}

	// 获取包围盒的顶点
	glm::vec3 getCorner(size_t index)const{
		glm::vec3 corner = m_max;
		// 根据下标的二进制位对应到某个顶点
		if ((index & 0b1) == 0) corner.x = m_min.x;
		if ((index & 0b10) == 0) corner.y = m_min.y;
		if ((index & 0b100) == 0) corner.z = m_min.z;
		return corner;
	}

	// 判断包围盒是否可用
	bool isValid() const{
		return m_max.x > m_min.x && m_max.y > m_min.y && m_max.z > m_min.z;
	}
};