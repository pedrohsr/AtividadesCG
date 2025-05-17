#include "Obj.hpp"
#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <iostream>

Obj::Obj(const std::string& filename) 
    : position(0.0f)
    , rotation(0.0f)
    , scale(1.0f)
    , VAO(0)
    , VBO(0)
    , EBO(0) {
    if (!loadFromFile(filename)) {
        std::cerr << "Failed to load model: " << filename << std::endl;
    }
    setupBuffers();
}

Obj::~Obj() {
    cleanup();
}

void Obj::cleanup() {
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (EBO != 0) glDeleteBuffers(1, &EBO);
}

bool Obj::loadFromFile(const std::string& filename) {
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;
    std::vector<unsigned int> vertexIndices, normalIndices;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if (type == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (type == "f") {
            std::string vertex1, vertex2, vertex3;
            iss >> vertex1 >> vertex2 >> vertex3;

            std::replace(vertex1.begin(), vertex1.end(), '/', ' ');
            std::replace(vertex2.begin(), vertex2.end(), '/', ' ');
            std::replace(vertex3.begin(), vertex3.end(), '/', ' ');

            std::istringstream v1(vertex1), v2(vertex2), v3(vertex3);
            unsigned int vertexIndex, normalIndex;

            v1 >> vertexIndex;
            vertexIndices.push_back(vertexIndex - 1);
            v1 >> normalIndex;
            v1 >> normalIndex;
            normalIndices.push_back(normalIndex - 1);

            v2 >> vertexIndex;
            vertexIndices.push_back(vertexIndex - 1);
            v2 >> normalIndex;
            v2 >> normalIndex;
            normalIndices.push_back(normalIndex - 1);

            v3 >> vertexIndex;
            vertexIndices.push_back(vertexIndex - 1);
            v3 >> normalIndex;
            v3 >> normalIndex;
            normalIndices.push_back(normalIndex - 1);
        }
    }

    vertices.clear();
    normals.clear();
    indices = vertexIndices;

    for (unsigned int i = 0; i < vertexIndices.size(); i++) {
        glm::vec3 vertex = temp_vertices[vertexIndices[i]];
        glm::vec3 normal = temp_normals[normalIndices[i]];
        
        vertices.push_back(vertex.x);
        vertices.push_back(vertex.y);
        vertices.push_back(vertex.z);
        
        normals.push_back(normal.x);
        normals.push_back(normal.y);
        normals.push_back(normal.z);
    }

    return true;
}

void Obj::setupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 
                 vertices.size() * sizeof(float) + normals.size() * sizeof(float),
                 nullptr, 
                 GL_STATIC_DRAW);
    
    glBufferSubData(GL_ARRAY_BUFFER, 
                    0, 
                    vertices.size() * sizeof(float), 
                    vertices.data());
    
    glBufferSubData(GL_ARRAY_BUFFER, 
                    vertices.size() * sizeof(float),
                    normals.size() * sizeof(float),
                    normals.data());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 indices.size() * sizeof(unsigned int),
                 indices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 
                         (void*)(vertices.size() * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Obj::translate(const glm::vec3& translation) {
    position += translation;
}

void Obj::rotate(float angle, const glm::vec3& axis) {
    rotation += axis * angle;
}

void Obj::setScale(const glm::vec3& newScale) {
    scale = newScale;
}

glm::mat4 Obj::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    return model;
}

void Obj::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
} 