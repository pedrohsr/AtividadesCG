#include "TexturedObj.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

TexturedObj::TexturedObj(const std::string &filename)
    : Obj(filename), texturedVAO(0), texturedVBO(0)
{

    if (loadTexturedOBJ(filename))
    {
        setupTexturedMesh();
        std::cout << "Loaded textured model: " << filename << std::endl;
    }
    else
    {
        std::cout << "Using basic model without textures: " << filename << std::endl;
    }
}

TexturedObj::~TexturedObj()
{
    if (texturedVAO != 0)
        glDeleteVertexArrays(1, &texturedVAO);
    if (texturedVBO != 0)
        glDeleteBuffers(1, &texturedVBO);

    for (auto &pair : materials)
    {
        if (pair.second.textureID != 0)
        {
            glDeleteTextures(1, &pair.second.textureID);
        }
    }
}

bool TexturedObj::loadTexturedOBJ(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        return false;
    }

    std::string line;
    std::string currentMaterial = "";

    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "mtllib")
        {
            std::string mtlFile;
            ss >> mtlFile;
            std::string mtlPath = path.substr(0, path.find_last_of("/\\") + 1) + mtlFile;
            loadMTL(mtlPath);
        }
        else if (prefix == "v")
        {
            glm::vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        }
        else if (prefix == "vt")
        {
            glm::vec2 texCoord;
            ss >> texCoord.x >> texCoord.y;
            texCoords.push_back(texCoord);
        }
        else if (prefix == "vn")
        {
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (prefix == "usemtl")
        {
            ss >> currentMaterial;
        }
        else if (prefix == "f")
        {
            Face face;
            face.material = currentMaterial;

            std::string vertex1, vertex2, vertex3;
            ss >> vertex1 >> vertex2 >> vertex3;

            auto parseVertex = [](const std::string &vertexStr, unsigned int &v, unsigned int &vt, unsigned int &vn)
            {
                std::string vertexCopy = vertexStr;
                std::replace(vertexCopy.begin(), vertexCopy.end(), '/', ' ');
                std::istringstream vss(vertexCopy);
                vss >> v >> vt >> vn;
                v--;
                vt--;
                vn--;
            };

            parseVertex(vertex1, face.v1, face.vt1, face.vn1);
            parseVertex(vertex2, face.v2, face.vt2, face.vn2);
            parseVertex(vertex3, face.v3, face.vt3, face.vn3);

            faces.push_back(face);
        }
    }

    file.close();
    return !vertices.empty() && !faces.empty();
}

bool TexturedObj::loadMTL(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cout << "Warning: Cannot open MTL file: " << path << std::endl;
        return false;
    }

    std::string line;
    Material currentMaterial;
    std::string currentMaterialName = "";

    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "newmtl")
        {
            if (!currentMaterialName.empty())
            {
                materials[currentMaterialName] = currentMaterial;
            }
            ss >> currentMaterialName;
            currentMaterial = Material();
            currentMaterial.name = currentMaterialName;
        }
        else if (prefix == "Ka")
        {
            ss >> currentMaterial.ambient.x >> currentMaterial.ambient.y >> currentMaterial.ambient.z;
        }
        else if (prefix == "Kd")
        {
            ss >> currentMaterial.diffuse.x >> currentMaterial.diffuse.y >> currentMaterial.diffuse.z;
        }
        else if (prefix == "Ks")
        {
            ss >> currentMaterial.specular.x >> currentMaterial.specular.y >> currentMaterial.specular.z;
        }
        else if (prefix == "Ns")
        {
            ss >> currentMaterial.shininess;
        }
        else if (prefix == "map_Kd")
        {
            std::string texturePath;
            ss >> texturePath;
            currentMaterial.diffuseTexture = texturePath;

            std::string fullTexturePath = path.substr(0, path.find_last_of("/\\") + 1) + texturePath;
            int imgWidth, imgHeight;
            currentMaterial.textureID = loadTexture(fullTexturePath, imgWidth, imgHeight);
        }
    }

    if (!currentMaterialName.empty())
    {
        materials[currentMaterialName] = currentMaterial;
    }

    file.close();
    return true;
}

GLuint TexturedObj::loadTexture(const std::string &filePath, int &width, int &height)
{
    GLuint texID;

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int nrChannels;
    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        if (nrChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Loaded texture: " << filePath << " (" << width << "x" << height << ", " << nrChannels << " channels)" << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture: " << filePath << std::endl;
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;
}

void TexturedObj::setupTexturedMesh()
{
    processedVertices.clear();

    for (const Face &face : faces)
    {
        TextureVertex v1;
        v1.position = (face.v1 < vertices.size()) ? vertices[face.v1] : glm::vec3(0.0f);
        v1.texCoord = (face.vt1 < texCoords.size()) ? texCoords[face.vt1] : glm::vec2(0.0f);
        v1.normal = (face.vn1 < normals.size()) ? normals[face.vn1] : glm::vec3(0.0f, 1.0f, 0.0f);
        processedVertices.push_back(v1);

        TextureVertex v2;
        v2.position = (face.v2 < vertices.size()) ? vertices[face.v2] : glm::vec3(0.0f);
        v2.texCoord = (face.vt2 < texCoords.size()) ? texCoords[face.vt2] : glm::vec2(0.0f);
        v2.normal = (face.vn2 < normals.size()) ? normals[face.vn2] : glm::vec3(0.0f, 1.0f, 0.0f);
        processedVertices.push_back(v2);

        TextureVertex v3;
        v3.position = (face.v3 < vertices.size()) ? vertices[face.v3] : glm::vec3(0.0f);
        v3.texCoord = (face.vt3 < texCoords.size()) ? texCoords[face.vt3] : glm::vec2(0.0f);
        v3.normal = (face.vn3 < normals.size()) ? normals[face.vn3] : glm::vec3(0.0f, 1.0f, 0.0f);
        processedVertices.push_back(v3);
    }

    if (!processedVertices.empty())
    {
        glGenVertexArrays(1, &texturedVAO);
        glGenBuffers(1, &texturedVBO);

        glBindVertexArray(texturedVAO);

        glBindBuffer(GL_ARRAY_BUFFER, texturedVBO);
        glBufferData(GL_ARRAY_BUFFER, processedVertices.size() * sizeof(TextureVertex), &processedVertices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (void *)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (void *)offsetof(TextureVertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (void *)offsetof(TextureVertex, texCoord));

        glBindVertexArray(0);
    }
}

void TexturedObj::drawTextured(GLuint shaderProgram) const
{
    if (texturedVAO == 0)
        return;

    bool hasTexture = false;
    for (const auto &pair : materials)
    {
        if (pair.second.textureID != 0)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pair.second.textureID);
            glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse1"), 0);
            hasTexture = true;
            break;
        }
    }

    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), hasTexture);

    glBindVertexArray(texturedVAO);
    glDrawArrays(GL_TRIANGLES, 0, processedVertices.size());
    glBindVertexArray(0);
}

bool TexturedObj::hasTextures() const
{
    for (const auto &pair : materials)
    {
        if (pair.second.textureID != 0)
        {
            return true;
        }
    }
    return false;
}

void TexturedObj::drawWithTextures() const
{
    if (hasTextures() && texturedVAO != 0)
    {
        glBindVertexArray(texturedVAO);
        glDrawArrays(GL_TRIANGLES, 0, processedVertices.size());
        glBindVertexArray(0);
    }
    else
    {
        Obj::draw();
    }
}