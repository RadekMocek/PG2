#pragma once

#include <filesystem>
#include <map>

#include "Vertex.hpp"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"

#define HEGHTMAP_SCALE 0.1f

class Model
{
public:
    Model(const std::filesystem::path& path_main, const std::filesystem::path& path_tex, bool is_height_map = false);
    void Draw(ShaderProgram& shader);
    
    glm::vec3 position{};
    glm::vec3 scale{};
    glm::vec4 rotation{}; // axes xyz + angle
    float _distance_from_camera; // for sorting transparent objects
    std::map<std::pair<float, float>, float> _heights; // for heightmap collision
private:
    Mesh mesh;
    std::string name;

    std::vector<Vertex> mesh_vertices{};
    std::vector<GLuint> mesh_vertex_indices{};

    glm::mat4 mx_model{};
    glm::vec3 rotation_axes{};

    // Reading the file
    void LoadOBJFile(const std::filesystem::path& file_name);

    // HeightMap
    void HeightMap_Load(const std::filesystem::path& file_name);
    glm::vec2 HeightMap_GetSubtexST(const int x, const int y);
    glm::vec2 HeightMap_GetSubtexByHeight(float height);
};
