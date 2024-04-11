#pragma once

#include <filesystem>

#include "Vertex.hpp"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"

class Model
{
public:
    Model(const std::filesystem::path& file_name, const std::filesystem::path& path_tex);
    void Draw(const ShaderProgram& shader);
private:
    Mesh mesh;
    std::string name;

    std::vector<Vertex> vertices{};
    std::vector<GLuint> vertex_indices{};

    glm::mat4 mx_model{};

    // Reading the file
    std::string file_line;
    std::vector<std::string> file_lines;
    void FillFileLines(const std::filesystem::path& path_obj);
    void LoadOBJFile(const std::filesystem::path& file_name, std::vector<Vertex>& out_vertices, std::vector<GLuint>& out_vertex_indices);
    void LoadMTLFile(const std::filesystem::path& file_name);
};
