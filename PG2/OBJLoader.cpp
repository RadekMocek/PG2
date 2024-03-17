#include <iostream>
#include <string>
#include <GL/glew.h> 
#include <glm/glm.hpp>

#include "OBJLoader.hpp"

#define MAX_LINE_SIZE 255

bool LoadOBJTest(const char* path, std::vector<Vertex>& out_vertices, std::vector<GLuint>& out_vertex_indices)
{
	std::vector<GLuint> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> indirect_vertices;
	std::vector<glm::vec2> indirect_uvs;
	std::vector<glm::vec3> indirect_normals;

	out_vertices.clear();
	out_vertex_indices.clear();

	FILE* file;
	fopen_s(&file, path, "r");
	if (file == NULL) {
		std::cout << "file == NULL\n";
		return false;
	}

	while (1) {
		char lineHeader[MAX_LINE_SIZE];
		int res = fscanf_s(file, "%s", lineHeader, MAX_LINE_SIZE);
		if (res == EOF) {
			break;
		}

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			indirect_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf_s(file, "%f %f\n", &uv.y, &uv.x);
			indirect_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			indirect_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches == 9) { //TODO 12 matches (012, 023)
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
			else {
				std::cout << "File can't be read by simple parser, matches=" << matches << " :(\n";
				return false;
			}
		}
	}

	out_vertex_indices = vertexIndices;

	auto n_indirect_vertices = indirect_vertices.size();
	auto n_indirect_uvs = indirect_uvs.size();
	auto n_indirect_normals = indirect_normals.size();

	for (unsigned int u = 0; u < n_indirect_vertices; u++) {
		Vertex vertex{};
		vertex.Position = indirect_vertices[u];
		if (u < n_indirect_uvs) vertex.TexCoords = indirect_uvs[u];
		if (u < n_indirect_normals) vertex.Normal = indirect_normals[u];
		out_vertices.push_back(vertex);
	}

	// unroll from indirect to direct vertex specification
	// sometimes not necessary, definitely not optimal
	/*
	std::vector<glm::vec3> direct_vertices{};
	std::vector<glm::vec2> direct_uvs{};
	std::vector<glm::vec3> direct_normals{};

	auto n_vertexIndices = vertexIndices.size();
	auto n_uvIndices = uvIndices.size();
	auto n_normalIndices = normalIndices.size();

	for (unsigned int u = 0; u < n_vertexIndices; u++) {
		unsigned int vertexIndex = vertexIndices[u];
		glm::vec3 vertex = indirect_vertices[vertexIndex - 1];
		direct_vertices.push_back(vertex);
	}
	for (unsigned int u = 0; u < n_uvIndices; u++) {
		unsigned int uvIndex = uvIndices[u];
		glm::vec2 uv = indirect_uvs[uvIndex - 1];
		direct_uvs.push_back(uv);
	}
	for (unsigned int u = 0; u < n_normalIndices; u++) {
		unsigned int normalIndex = normalIndices[u];
		glm::vec3 normal = indirect_normals[normalIndex - 1];
		direct_normals.push_back(normal);
	}

	auto n_direct_vertices = direct_vertices.size();
	auto n_direct_uvs = direct_uvs.size();
	auto n_direct_normals = direct_normals.size();

	for (unsigned int u = 0; u < n_direct_vertices; u++) {
		Vertex vertex{};
		vertex.Position = direct_vertices[u];
		if (u < n_direct_uvs) vertex.TexCoords = direct_uvs[u];
		if (u < n_direct_normals) vertex.Normal = direct_normals[u];
		out_vertices.push_back(vertex);
	}
	/**/

	fclose(file);
	return true;
}