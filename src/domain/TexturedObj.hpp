#ifndef TEXTURED_OBJ_H
#define TEXTURED_OBJ_H

#include "Obj.hpp"
#include <map>
#include <string>

struct Material
{
    std::string name;
    std::string diffuseTexture;
    glm::vec3 ambient = glm::vec3(0.2f);
    glm::vec3 diffuse = glm::vec3(0.8f);
    glm::vec3 specular = glm::vec3(1.0f);
    float shininess = 32.0f;
    GLuint textureID = 0;
};

struct TextureVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

struct Face
{
    unsigned int v1, v2, v3;
    unsigned int vt1, vt2, vt3;
    unsigned int vn1, vn2, vn3;
    std::string material;
};

class TexturedObj : public Obj
{
private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    std::vector<Face> faces;
    std::map<std::string, Material> materials;

    GLuint texturedVAO, texturedVBO;
    std::vector<TextureVertex> processedVertices;

    bool loadTexturedOBJ(const std::string &filename);
    bool loadMTL(const std::string &path);
    GLuint loadTexture(const std::string &filePath, int &width, int &height);
    void setupTexturedMesh();

public:
    TexturedObj(const std::string &filename);
    ~TexturedObj();

    void drawTextured(GLuint shaderProgram) const;
    bool hasTextures() const;

    void drawWithTextures() const;
};

#endif