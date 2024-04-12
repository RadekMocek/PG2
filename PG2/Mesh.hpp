#pragma once

//#include <glm/glm.hpp> 
#include <glm/ext.hpp>
//#include <string>
//#include <vector>

#include <GL/glew.h>

#include "ShaderProgram.hpp"
#include "Vertex.hpp"

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    GLuint texture_id{ 0 }; // texture id=0  means no texture
    GLenum primitive_type = GL_POINTS;

    Mesh(GLenum primitive_type, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint texture_id);
    void Draw(ShaderProgram& shader);
    void Draw(ShaderProgram& shader, glm::mat4 mx_model);
    void Clear(void);

    // Tell the compiler to do what it would have if we didn't define a ctor:
    Mesh() = default;
private:
    // OpenGL buffer IDs
    // ID = 0 is reserved (i.e. uninitalized)
    unsigned int VAO{ 0 }, VBO{ 0 }, EBO{ 0 };
};
