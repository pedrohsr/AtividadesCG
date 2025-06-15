#include "Camera.hpp"
#include <algorithm>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : position(position), worldUp(up), yaw(yaw), pitch(pitch),
      front(glm::vec3(0.0f, 0.0f, -1.0f)),
      movementSpeed(2.5f),
      mouseSensitivity(0.1f)
{
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
}

void Camera::MoveForward(float deltaTime)
{
    position += front * movementSpeed * deltaTime;
}

void Camera::MoveBackward(float deltaTime)
{
    position -= front * movementSpeed * deltaTime;
}

void Camera::MoveLeft(float deltaTime)
{
    position -= right * movementSpeed * deltaTime;
}

void Camera::MoveRight(float deltaTime)
{
    position += right * movementSpeed * deltaTime;
}

void Camera::MoveUp(float deltaTime)
{
    position += up * movementSpeed * deltaTime;
}

void Camera::MoveDown(float deltaTime)
{
    position -= up * movementSpeed * deltaTime;
}

void Camera::Rotate(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (constrainPitch)
    {
        pitch = std::clamp(pitch, -89.0f, 89.0f);
    }

    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}