#include "Obj.hpp"
#include "glad/glad.h"
#include <fstream>
#include <sstream>
#include <iostream>

Obj::Obj(const std::string& filename) 
    : position(0.0f)
    , rotation(0.0f)
    , scale(1.0f)
    , VAO(0)
    , VBO(0)
    , numVertices(0) {
    if (!loadFromFile(filename)) {
        std::cerr << "Failed to load model: " << filename << std::endl;
    }
}

Obj::~Obj() {
    cleanup();
}

void Obj::cleanup() {
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
}

bool Obj::loadFromFile(const std::string& filename) {
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;
    std::vector<float> vBuffer;

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

            std::string vertices[3] = {vertex1, vertex2, vertex3};
            for (const auto& vertex : vertices) {
                std::istringstream ss(vertex);
                std::string index;
                int vi = 0, ni = 0;

                if (std::getline(ss, index, '/')) {
                    vi = !index.empty() ? std::stoi(index) - 1 : 0;
                }

                std::getline(ss, index, '/');

                if (std::getline(ss, index)) {
                    ni = !index.empty() ? std::stoi(index) - 1 : 0;
                }

                vBuffer.push_back(temp_vertices[vi].x);
                vBuffer.push_back(temp_vertices[vi].y);
                vBuffer.push_back(temp_vertices[vi].z);

                vBuffer.push_back(temp_normals[ni].x);
                vBuffer.push_back(temp_normals[ni].y);
                vBuffer.push_back(temp_normals[ni].z);
            }
        }
    }

    if (vBuffer.empty()) {
        std::cerr << "Error: No valid faces found in file: " << filename << std::endl;
        return false;
    }

    numVertices = vBuffer.size() / 6;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vBuffer.size() * sizeof(float), vBuffer.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}

void Obj::translate(const glm::vec3& translation) {
    position += translation;
}

void Obj::rotate(float angle, const glm::vec3& axis) {
    rotation += axis * angle;
}

void Obj::setRotation(const glm::vec3& newRotation) {
    rotation = newRotation;
}

void Obj::setScale(const glm::vec3& newScale) {
    scale = newScale;
}

glm::mat4 Obj::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, scale);
    return model;
}

glm::vec3 Obj::getRotation() const {
    return rotation;
}

void Obj::draw() const {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
    glBindVertexArray(0);
} 