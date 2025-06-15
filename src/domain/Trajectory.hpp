#ifndef TRAJECTORY_HPP
#define TRAJECTORY_HPP

#include <vector>
#include <glm/glm.hpp>

class Trajectory
{
public:
    Trajectory() : currentPoint(0), speed(1.0f) {}

    void addPoint(const glm::vec3 &point)
    {
        controlPoints.push_back(point);
    }

    glm::vec3 getNextPosition(float deltaTime)
    {
        if (controlPoints.empty())
        {
            return glm::vec3(0.0f);
        }

        glm::vec3 target = controlPoints[currentPoint];

        glm::vec3 direction = target - currentPosition;
        float distance = glm::length(direction);

        if (distance < 0.1f)
        {
            currentPoint = (currentPoint + 1) % controlPoints.size();
        }
        else
        {
            direction = glm::normalize(direction);
            currentPosition += direction * speed * deltaTime;
        }

        return currentPosition;
    }

    void clear()
    {
        controlPoints.clear();
        currentPoint = 0;
        currentPosition = glm::vec3(0.0f);
    }

    const std::vector<glm::vec3> &getControlPoints() const
    {
        return controlPoints;
    }

    void setSpeed(float newSpeed)
    {
        speed = newSpeed;
    }

private:
    std::vector<glm::vec3> controlPoints;
    size_t currentPoint;
    glm::vec3 currentPosition;
    float speed;
};

#endif