#ifndef OBJ_H
#define OBJ_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>

class Obj {
public:
    Obj(const std::string& filename);
    ~Obj();

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<unsigned int> indices;
    
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    
    unsigned int VAO, VBO, EBO;
    
    void translate(const glm::vec3& translation);
    void rotate(float angle, const glm::vec3& axis);
    void setScale(const glm::vec3& newScale);
    
    glm::mat4 getModelMatrix() const;
    
    bool loadFromFile(const std::string& filename);
    
    void setupBuffers();
    
    void draw() const;

private:
    void cleanup();
};

#endif