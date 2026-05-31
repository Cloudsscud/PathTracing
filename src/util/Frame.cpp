#include "util/Frame.h"

Frame::Frame(const glm::vec3& normal) {
	m_y_axis = normal;	// 局部坐标系中y为法线

	glm::vec3 up = normal.y < 0.99999f ? glm::vec3{ 0, 1, 0 } : glm::vec3{ 0, 0, 1 };	// 防止y轴为0
	m_x_axis = glm::cross(up, normal);	// x,z构成切面
	m_z_axis = glm::cross(m_x_axis, m_y_axis);
}

glm::vec3 Frame::localFromWorld(const glm::vec3& world) {
	return glm::normalize(glm::vec3{
		glm::dot(world, m_x_axis),
		glm::dot(world, m_y_axis),
		glm::dot(world, m_z_axis) });
}
glm::vec3 Frame::worldFromLocal(const glm::vec3& local) {
	return glm::normalize(local.x * m_x_axis + local.y * m_y_axis + local.z * m_z_axis);
}