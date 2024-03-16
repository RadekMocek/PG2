///*

#include <filesystem>

#include "Vertex.hpp"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"

class Model
{
public:
    Model(const std::filesystem::path& filename);
    void Draw(const ShaderProgram& shader);
private:
    std::vector<Mesh> meshes;
    std::string name;
    void LoadOBJFile(const std::filesystem::path& filename);
    void LoadMTLFile(const std::filesystem::path& filename);
    //GLuint LoadTexture(const std::filesystem::path& filename);
    //...
    //TODO

    std::vector<glm::vec3> vertices{};
    std::vector<GLuint> vertex_indices{};
    //std::vector<glm::vec2> uvs{};
    //std::vector<glm::vec3> normals{};
};

/**/
