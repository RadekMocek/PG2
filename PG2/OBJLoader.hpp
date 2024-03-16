#pragma once
#ifndef OBJLoader_HPP
#define OBJLoader_HPP

#include <vector>
#include <glm/fwd.hpp>

#include <GL/glew.h> // Haluz

bool LoadOBJ(
	const char* path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
);

bool LoadOBJTest(const char* path, std::vector<glm::vec3>& out_vertices, std::vector<GLuint>& out_vertex_indices);

#endif
