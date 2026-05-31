#pragma once

#include <glm/glm.hpp>

// 基于交点法线的局部坐标系
class Frame {
public:
	Frame(const glm::vec3& normal);

	glm::vec3 localFromWorld(const glm::vec3& world);
	glm::vec3 worldFromLocal(const glm::vec3& local);
private:
	glm::vec3 m_x_axis, m_y_axis, m_z_axis;
};