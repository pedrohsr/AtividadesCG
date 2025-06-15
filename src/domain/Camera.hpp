#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f,
           float pitch = 0.0f);

    glm::mat4 GetViewMatrix() const;

    void MoveForward(float deltaTime);
    void MoveBackward(float deltaTime);
    void MoveLeft(float deltaTime);
    void MoveRight(float deltaTime);
    void MoveUp(float deltaTime);
    void MoveDown(float deltaTime);

    void Rotate(float xoffset, float yoffset, bool constrainPitch = true);

    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetFront() const { return front; }
    float GetYaw() const { return yaw; }
    float GetPitch() const { return pitch; }

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;

    float movementSpeed;
    float mouseSensitivity;

    void updateCameraVectors();
};

#endif