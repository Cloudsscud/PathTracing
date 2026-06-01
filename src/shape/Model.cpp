#include "shape/Model.h"
#include "util/Timer.h"
#include <iostream>
#include <fstream>
#include <sstream>


Model::Model(const std::filesystem::path& filename) {
	TIMER("load model " + filename.string());

	// 模型文件为 .obj格式
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;

	std::ifstream file(filename);
	if (!file.good()) {
		std::cout << "文件读取失败" << std::endl;
		return;
	}

	std::string line;
	char trash;
	while (!file.eof()) {
		std::getline(file, line);
		std::istringstream iss(line);

		if (line.compare(0, 2, "v ") == 0) {
			glm::vec3 position;
			iss >> trash >> position.x >> position.y >> position.z;	// v p0 p1 p2
			positions.push_back(position);
		}
		else if (line.compare(0, 3, "vn ") == 0) {
			glm::vec3 normal;
			iss >> trash >> trash >> normal.x >> normal.y >> normal.z; // vn n0 n1 n2
			normals.push_back(normal);
		}
		else if (line.compare(0, 2, "f ") == 0) {
			// f p0/vn0 p1/vn1 p2/vn2 
			glm::ivec3 idx_v, idx_vn;
			iss >> trash;
			iss >> idx_v.x >> trash >> trash >> idx_vn.x;
			iss >> idx_v.y >> trash >> trash >> idx_vn.y;
			iss >> idx_v.z >> trash >> trash >> idx_vn.z;
			// obj 从1开始
			m_triangles.push_back(Triangle(
				positions[idx_v.x - 1], positions[idx_v.y - 1], positions[idx_v.z - 1],
				normals[idx_vn.x - 1], normals[idx_vn.y - 1], normals[idx_vn.z - 1]
			));
		}
	}
	build();	// 建立AABB包围盒
}

std::optional<HitInfo> Model::intersect(const Ray& ray, float tmin,	float tmax) const {
	if (!m_box.hasIntersection(ray, tmin, tmax)) {
		return {};
	}

	std::optional<HitInfo> closest_hit_info = {};

	for (const auto& triangle : m_triangles) {
		auto hit_info = triangle.intersect(ray, tmin, tmax);
		if (hit_info.has_value()) {
			tmax = hit_info->m_hit_t;
			closest_hit_info = hit_info;
		}
	}
	return closest_hit_info;
}

void Model::build() {
	for (const auto& triangle : m_triangles) {
		m_box.expand(triangle.m_p0);
		m_box.expand(triangle.m_p1);
		m_box.expand(triangle.m_p2);
	}
}