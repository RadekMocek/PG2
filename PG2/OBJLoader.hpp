#pragma once
#ifndef OBJLoader_HPP
#define OBJLoader_HPP

#include <vector>
#include <glm/fwd.hpp>

#include <GL/glew.h> // Haluz

#include "Vertex.hpp"

bool LoadOBJTest(const char* path, std::vector<Vertex>& out_vertices, std::vector<GLuint>& out_vertex_indices);

#endif
