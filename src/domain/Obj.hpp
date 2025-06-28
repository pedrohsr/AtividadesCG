#ifndef OBJ_H
#define OBJ_H

#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>

class Obj {
private:
    GLuint VAO;
    GLuint VBO;
    int numVertices;

    glm::vec3 position;
    glm::vec3 rotation;

    bool loadFromFile(const std::string& filename);
    void cleanup();

public:
    Obj(const std::string& filename);
    ~Obj();

    glm::vec3 scale;

    void translate(const glm::vec3& translation);
    void rotate(float angle, const glm::vec3& axis);
    void setRotation(const glm::vec3& newRotation);
    void setScale(const glm::vec3& newScale);
    glm::mat4 getModelMatrix() const;
    glm::vec3 getRotation() const;
    
    void draw() const;
};

#endif