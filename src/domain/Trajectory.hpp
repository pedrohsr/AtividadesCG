#ifndef TRAJECTORY_HPP
#define TRAJECTORY_HPP

#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <iostream>

class Trajectory
{
public:
    Trajectory() : currentPoint(0), speed(1.0f) {}

    void addPoint(const glm::vec3 &point)
    {
        controlPoints.push_back(point);
        std::cout << "Added point " << controlPoints.size() << ": (" 
                  << point.x << ", " << point.y << ", " << point.z << ")" << std::endl;
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
            std::cout << "Reached point " << (currentPoint == 0 ? controlPoints.size() : currentPoint) 
                      << " of " << controlPoints.size() << std::endl;
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
        std::cout << "Trajectory cleared" << std::endl;
    }

    const std::vector<glm::vec3> &getControlPoints() const
    {
        return controlPoints;
    }

    void setSpeed(float newSpeed)
    {
        speed = newSpeed;
        std::cout << "Speed set to: " << speed << std::endl;
    }

    float getSpeed() const
    {
        return speed;
    }

    bool saveToFile(const std::string& filename) const
    {
        std::ofstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file for writing: " << filename << std::endl;
            return false;
        }

        file << controlPoints.size() << std::endl;
        for (const auto& point : controlPoints)
        {
            file << point.x << " " << point.y << " " << point.z << std::endl;
        }
        
        std::cout << "Trajectory saved to: " << filename << std::endl;
        return true;
    }

    bool loadFromFile(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file for reading: " << filename << std::endl;
            return false;
        }

        clear();
        int numPoints;
        file >> numPoints;
        
        for (int i = 0; i < numPoints; i++)
        {
            glm::vec3 point;
            file >> point.x >> point.y >> point.z;
            controlPoints.push_back(point);
        }
        
        std::cout << "Trajectory loaded from: " << filename << " (" << numPoints << " points)" << std::endl;
        return true;
    }

    void printInfo() const
    {
        std::cout << "Trajectory has " << controlPoints.size() << " control points:" << std::endl;
        for (size_t i = 0; i < controlPoints.size(); i++)
        {
            const auto& point = controlPoints[i];
            std::cout << "  Point " << (i+1) << ": (" << point.x << ", " << point.y << ", " << point.z << ")" << std::endl;
        }
    }

private:
    std::vector<glm::vec3> controlPoints;
    size_t currentPoint;
    glm::vec3 currentPosition;
    float speed;
};

#endif