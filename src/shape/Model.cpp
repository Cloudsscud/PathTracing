#include "shape/Model.h"
#include "util/Timer.h"
#include <rapidobj/rapidobj.hpp>


Model::Model(const std::filesystem::path& filename) {
	TIMER("load model " + filename.string());

	// rapidobj º”‘ÿƒ£–Õ
	auto result = rapidobj::ParseFile(filename, rapidobj::MaterialLibrary::Ignore());

	std::vector<Triangle> tri;
	for (const auto& shape : result.shapes) {
		size_t index_offset = 0;
		for (size_t num_face_vertices : shape.mesh.num_face_vertices) {
			if (num_face_vertices == 3) {
				auto index = shape.mesh.indices[index_offset];
				glm::vec3 pos0 = {
					result.attributes.positions[index.position_index * 3 + 0],
					result.attributes.positions[index.position_index * 3 + 1],
					result.attributes.positions[index.position_index * 3 + 2]
				};
				index = shape.mesh.indices[index_offset+1];
				glm::vec3 pos1 = {
					result.attributes.positions[index.position_index * 3 + 0],
					result.attributes.positions[index.position_index * 3 + 1],
					result.attributes.positions[index.position_index * 3 + 2]
				};
				index = shape.mesh.indices[index_offset+2];
				glm::vec3 pos2 = {
					result.attributes.positions[index.position_index * 3 + 0],
					result.attributes.positions[index.position_index * 3 + 1],
					result.attributes.positions[index.position_index * 3 + 2]
				};

				bool has_valid_normal = false;

				if (index.normal_index >= 0) {
					auto index = shape.mesh.indices[index_offset];
					glm::vec3 nor0 = {
						result.attributes.normals[index.normal_index * 3 + 0],
						result.attributes.normals[index.normal_index * 3 + 1],
						result.attributes.normals[index.normal_index * 3 + 2]
					};
					index = shape.mesh.indices[index_offset + 1];
					glm::vec3 nor1 = {
						result.attributes.normals[index.normal_index * 3 + 0],
						result.attributes.normals[index.normal_index * 3 + 1],
						result.attributes.normals[index.normal_index * 3 + 2]
					};
					index = shape.mesh.indices[index_offset + 2];
					glm::vec3 nor2 = {
						result.attributes.normals[index.normal_index * 3 + 0],
						result.attributes.normals[index.normal_index * 3 + 1],
						result.attributes.normals[index.normal_index * 3 + 2]
					};

					if(glm::length(nor0) > 0.001f &&
						glm::length(nor1) > 0.001f &&
						glm::length(nor2) > 0.001f){
						has_valid_normal = true;
						tri.push_back(Triangle{ pos0, pos1,pos2, 
												glm::normalize(nor0),
												glm::normalize(nor1),
												glm::normalize(nor2) });
					}
				}
				if (!has_valid_normal) {
					tri.push_back(Triangle{ pos0, pos1,pos2 });
				}
			}
			index_offset += num_face_vertices;
		}
	}

	m_bvh.build(std::move(tri));
}

std::optional<HitInfo> Model::intersect(const Ray& ray, float tmin,	float tmax) const {
	return m_bvh.intersect(ray, tmin, tmax);
}