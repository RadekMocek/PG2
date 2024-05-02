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
    std::string name;

    Model(std::string name, const std::filesystem::path& path_main, const std::filesystem::path& path_tex, glm::vec3 position, float scale, glm::vec4 init_rotation, bool is_height_map, bool use_aabb);
    void Draw(ShaderProgram& shader);
    
    // Transformation
    glm::vec3 position{};    
    float scale{};
    glm::vec4 rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); // axes xyz + angle

    //
    float _distance_from_camera; // for sorting transparent objects
    std::map<std::pair<float, float>, float> _heights; // for heightmap collision

    // Collision
    bool use_aabb;
    // - Bounding sphere
    glm::vec3 coll_center{};
    float coll_radius{};
    // - AABB
    glm::vec3 aabb_min{};
    glm::vec3 aabb_max{};
private:
    Mesh mesh;

    std::vector<Vertex> mesh_vertices{};
    std::vector<GLuint> mesh_vertex_indices{};

    glm::mat4 mx_model{};
    glm::vec3 rotation_axes{};
    glm::vec3 init_rotation_axes{};

    // Transformation
    glm::vec4 init_rotation{}; // axes xyz + angle

    // OBJ file
    void LoadOBJFile(const std::filesystem::path& file_name);

    // HeightMap
    void HeightMap_Load(const std::filesystem::path& file_name);
    glm::vec2 HeightMap_GetSubtexST(const int x, const int y);
    glm::vec2 HeightMap_GetSubtexByHeight(float height);
};
