#ifndef TRAJECTORY_HPP
#define TRAJECTORY_HPP

#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>

enum class InterpolationType {
    LINEAR,
    BEZIER,
    SPLINE
};

class Trajectory
{
public:
    Trajectory() : currentPoint(0), speed(1.0f), interpolationType(InterpolationType::LINEAR), 
                   currentTime(0.0f), totalTime(0.0f) {}

    void addPoint(const glm::vec3 &point)
    {
        controlPoints.push_back(point);
        std::cout << "Added point " << controlPoints.size() << ": (" 
                  << point.x << ", " << point.y << ", " << point.z << ")" << std::endl;
        updateTotalTime();
    }

    void setInterpolationType(InterpolationType type)
    {
        interpolationType = type;
        std::cout << "Interpolation type set to: " << (type == InterpolationType::LINEAR ? "LINEAR" : 
                                                       type == InterpolationType::BEZIER ? "BEZIER" : "SPLINE") << std::endl;
    }

    InterpolationType getInterpolationType() const
    {
        return interpolationType;
    }

    glm::vec3 getNextPosition(float deltaTime)
    {
        if (controlPoints.empty())
        {
            return glm::vec3(0.0f);
        }

        if (controlPoints.size() == 1)
        {
            return controlPoints[0];
        }

        currentTime += speed * deltaTime;
        
        if (currentTime >= totalTime)
        {
            currentTime = 0.0f;
            std::cout << "Completed trajectory cycle" << std::endl;
        }

        float t = currentTime / totalTime;
        return interpolate(t);
    }

    void clear()
    {
        controlPoints.clear();
        currentPoint = 0;
        currentPosition = glm::vec3(0.0f);
        currentTime = 0.0f;
        totalTime = 0.0f;
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
        
        file << static_cast<int>(interpolationType) << std::endl;
        file << speed << std::endl;
        
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
        
        int typeInt;
        file >> typeInt;
        interpolationType = static_cast<InterpolationType>(typeInt);
        
        file >> speed;
        updateTotalTime();
        
        std::cout << "Trajectory loaded from: " << filename << " (" << numPoints << " points)" << std::endl;
        return true;
    }

    void printInfo() const
    {
        std::cout << "Trajectory has " << controlPoints.size() << " control points:" << std::endl;
        std::cout << "Interpolation type: " << (interpolationType == InterpolationType::LINEAR ? "LINEAR" : 
                                               interpolationType == InterpolationType::BEZIER ? "BEZIER" : "SPLINE") << std::endl;
        std::cout << "Speed: " << speed << std::endl;
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
    InterpolationType interpolationType;
    float currentTime;
    float totalTime;

    void updateTotalTime()
    {
        if (controlPoints.size() < 2) return;
        
        switch (interpolationType)
        {
            case InterpolationType::LINEAR:
                totalTime = controlPoints.size();
                break;
            case InterpolationType::BEZIER:
                totalTime = controlPoints.size() - 1;
                break;
            case InterpolationType::SPLINE:
                totalTime = controlPoints.size() - 1;
                break;
        }
    }

    glm::vec3 interpolate(float t)
    {
        switch (interpolationType)
        {
            case InterpolationType::LINEAR:
                return linearInterpolation(t);
            case InterpolationType::BEZIER:
                return bezierInterpolation(t);
            case InterpolationType::SPLINE:
                return splineInterpolation(t);
            default:
                return linearInterpolation(t);
        }
    }

    glm::vec3 linearInterpolation(float t)
    {
        if (controlPoints.size() < 2) return controlPoints[0];
        
        float segmentTime = 1.0f / (controlPoints.size() - 1);
        int segment = static_cast<int>(t / segmentTime);
        segment = std::min(segment, static_cast<int>(controlPoints.size() - 2));
        
        float localT = (t - segment * segmentTime) / segmentTime;
        localT = glm::clamp(localT, 0.0f, 1.0f);
        
        return glm::mix(controlPoints[segment], controlPoints[segment + 1], localT);
    }

    glm::vec3 bezierInterpolation(float t)
    {
        if (controlPoints.size() < 2) return controlPoints[0];
        
        int n = controlPoints.size() - 1;
        glm::vec3 result(0.0f);
        
        for (int i = 0; i <= n; i++)
        {
            float coefficient = binomialCoefficient(n, i) * std::pow(1.0f - t, n - i) * std::pow(t, i);
            result += coefficient * controlPoints[i];
        }
        
        return result;
    }

    glm::vec3 splineInterpolation(float t)
    {
        if (controlPoints.size() < 4) return linearInterpolation(t);
        
        int n = controlPoints.size() - 1;
        float segmentTime = 1.0f / n;
        int segment = static_cast<int>(t / segmentTime);
        segment = std::min(segment, n - 1);
        
        float localT = (t - segment * segmentTime) / segmentTime;
        localT = glm::clamp(localT, 0.0f, 1.0f);
        
        int p0 = std::max(0, segment - 1);
        int p1 = segment;
        int p2 = segment + 1;
        int p3 = std::min(n, segment + 2);
        
        float t2 = localT * localT;
        float t3 = t2 * localT;
        
        glm::vec3 result = 
            (-0.5f * t3 + t2 - 0.5f * localT) * controlPoints[p0] +
            (1.5f * t3 - 2.5f * t2 + 1.0f) * controlPoints[p1] +
            (-1.5f * t3 + 2.0f * t2 + 0.5f * localT) * controlPoints[p2] +
            (0.5f * t3 - 0.5f * t2) * controlPoints[p3];
        
        return result;
    }

    int binomialCoefficient(int n, int k)
    {
        if (k > n - k) k = n - k;
        int result = 1;
        for (int i = 0; i < k; i++)
        {
            result *= (n - i);
            result /= (i + 1);
        }
        return result;
    }
};

#endif