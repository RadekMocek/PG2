///*

#include "OBJLoader.hpp"
#include "Model.hpp"

Model::Model(const std::filesystem::path& filename) {
    // load mesh (all meshes) of the model, load material of each mesh, load textures...
    // TODO: call LoadOBJFile, LoadMTLFile, process data, create mesh and set its properties
    bool is_loadobj_success = LoadOBJTest(filename.string().c_str(), vertices, vertex_indices);
    //Mesh mesh = Mesh(GL_POINT, vertices, vertex_indices, 0);
}

void Model::Draw(const ShaderProgram& shader) {
    // call Draw() on all meshes
    for (auto const& mesh : meshes) {
        mesh.Draw(shader);
    }
}

void Model::LoadOBJFile(const std::filesystem::path& filename)
{
    // TODO modify
}

void Model::LoadMTLFile(const std::filesystem::path& filename)
{
    // TODO
}

/**/
