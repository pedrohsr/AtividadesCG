#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include "domain/Shader.hpp"
#include "domain/TexturedObj.hpp"
#include "domain/Trajectory.hpp"

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct ObjectWithTrajectory
{
    TexturedObj *obj;
    Trajectory trajectory;
    bool isMoving;
};

std::vector<ObjectWithTrajectory> objects;
int selectedObject = 0;

bool addingControlPoint = false;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void printUsage(const char *programName);

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    static double lastX = SCR_WIDTH / 2.0f;
    static double lastY = SCR_HEIGHT / 2.0f;
    static bool firstMouse = true;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    static float yaw = -90.0f;
    static float pitch = 0.0f;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    static bool tabPressed = false;
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
    {
        if (!tabPressed && !objects.empty())
        {
            selectedObject = (selectedObject + 1) % objects.size();
            std::cout << "Selected object: " << selectedObject << std::endl;
            tabPressed = true;
        }
    }
    else
    {
        tabPressed = false;
    }

    if (!objects.empty() && selectedObject >= 0 && selectedObject < objects.size())
    {
        static bool cPressed = false;
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        {
            if (!cPressed)
            {
                addingControlPoint = !addingControlPoint;
                std::cout << (addingControlPoint ? "Adding control points" : "Stopped adding control points") << std::endl;
                cPressed = true;
            }
        }
        else
        {
            cPressed = false;
        }

        static bool xPressed = false;
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        {
            if (!xPressed)
            {
                objects[selectedObject].trajectory.clear();
                std::cout << "Cleared trajectory for object " << selectedObject << std::endl;
                xPressed = true;
            }
        }
        else
        {
            xPressed = false;
        }

        static bool mPressed = false;
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        {
            if (!mPressed)
            {
                objects[selectedObject].isMoving = !objects[selectedObject].isMoving;
                std::cout << "Object " << selectedObject << (objects[selectedObject].isMoving ? " started" : " stopped") << " movement" << std::endl;
                mPressed = true;
            }
        }
        else
        {
            mPressed = false;
        }

        static bool spacePressed = false;
        if (addingControlPoint && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            if (!spacePressed)
            {
                glm::mat4 modelMatrix = objects[selectedObject].obj->getModelMatrix();
                glm::vec3 currentPos(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
                objects[selectedObject].trajectory.addPoint(currentPos);
                spacePressed = true;
            }
        }
        else
        {
            spacePressed = false;
        }

        static bool f1Pressed = false;
        if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
        {
            if (!f1Pressed)
            {
                std::string filename = "trajectory_" + std::to_string(selectedObject) + ".txt";
                objects[selectedObject].trajectory.saveToFile(filename);
                f1Pressed = true;
            }
        }
        else
        {
            f1Pressed = false;
        }

        static bool f2Pressed = false;
        if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
        {
            if (!f2Pressed)
            {
                std::string filename = "trajectory_" + std::to_string(selectedObject) + ".txt";
                objects[selectedObject].trajectory.loadFromFile(filename);
                f2Pressed = true;
            }
        }
        else
        {
            f2Pressed = false;
        }

        static bool iPressed = false;
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        {
            if (!iPressed)
            {
                std::cout << "=== Trajectory Info for Object " << selectedObject << " ===" << std::endl;
                objects[selectedObject].trajectory.printInfo();
                std::cout << "Movement: " << (objects[selectedObject].isMoving ? "ON" : "OFF") << std::endl;
                std::cout << "=====================================" << std::endl;
                iPressed = true;
            }
        }
        else
        {
            iPressed = false;
        }

        static bool plusPressed = false;
        if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
        {
            if (!plusPressed)
            {
                float currentSpeed = objects[selectedObject].trajectory.getSpeed();
                objects[selectedObject].trajectory.setSpeed(currentSpeed + 0.5f);
                plusPressed = true;
            }
        }
        else
        {
            plusPressed = false;
        }

        static bool minusPressed = false;
        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
        {
            if (!minusPressed)
            {
                float currentSpeed = objects[selectedObject].trajectory.getSpeed();
                objects[selectedObject].trajectory.setSpeed(std::max(0.1f, currentSpeed - 0.5f));
                minusPressed = true;
            }
        }
        else
        {
            minusPressed = false;
        }

        float moveSpeed = 2.0f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            objects[selectedObject].obj->translate(glm::vec3(0.0f, moveSpeed, 0.0f));
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            objects[selectedObject].obj->translate(glm::vec3(0.0f, -moveSpeed, 0.0f));
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            objects[selectedObject].obj->translate(glm::vec3(-moveSpeed, 0.0f, 0.0f));
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            objects[selectedObject].obj->translate(glm::vec3(moveSpeed, 0.0f, 0.0f));
        if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
            objects[selectedObject].obj->translate(glm::vec3(0.0f, 0.0f, -moveSpeed));
        if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
            objects[selectedObject].obj->translate(glm::vec3(0.0f, 0.0f, moveSpeed));
    }
}

void printUsage(const char *programName)
{
    std::cout << "=== TRAJECTORY VIEWER ===" << std::endl;
    std::cout << "Usage: " << programName << " <model1.obj> [model2.obj] [model3.obj] ..." << std::endl;
    std::cout << "Tutorial: Press C, move object, press Space, press C, press M" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "- WASD: Move camera" << std::endl;
    std::cout << "- Mouse: Look around" << std::endl;
    std::cout << "- TAB: Switch between objects" << std::endl;
    std::cout << "- C: Toggle adding control points mode" << std::endl;
    std::cout << "- SPACE: Add control point at current position (when in adding mode)" << std::endl;
    std::cout << "- X: Clear trajectory of selected object" << std::endl;
    std::cout << "- M: Toggle object movement along trajectory" << std::endl;
    std::cout << "- Arrow keys: Move selected object manually" << std::endl;
    std::cout << "- Page Up/Down: Move selected object forward/backward" << std::endl;
    std::cout << "- F1: Save trajectory to file" << std::endl;
    std::cout << "- F2: Load trajectory from file" << std::endl;
    std::cout << "- I: Show trajectory info" << std::endl;
    std::cout << "- +/-: Adjust movement speed" << std::endl;
    std::cout << "- ESC: Exit" << std::endl;
    std::cout << "========================" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printUsage(argv[0]);
        return -1;
    }

    printUsage(argv[0]);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Trajectory Viewer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader texturedShader("src/shaders/textured.vert", "src/shaders/textured.frag");

    float xOffset = 0.0f;
    for (int i = 1; i < argc; i++)
    {
        try
        {
            TexturedObj *obj = new TexturedObj(argv[i]);
            obj->translate(glm::vec3(xOffset, 0.0f, 0.0f));
            ObjectWithTrajectory objWithTraj = {obj, Trajectory(), false};
            objects.push_back(objWithTraj);
            xOffset += 3.0f;
            std::cout << "Loaded model: " << argv[i] << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Failed to load model " << argv[i] << ": " << e.what() << std::endl;
        }
    }

    if (objects.empty())
    {
        std::cerr << "No valid models were loaded. Exiting.\n";
        glfwTerminate();
        return -1;
    }

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        texturedShader.use();

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        texturedShader.setMat4("view", view);
        texturedShader.setMat4("projection", projection);

        texturedShader.setVec3("lightPos", glm::vec3(5.0f, 5.0f, 5.0f));
        texturedShader.setVec3("lightColor", glm::vec3(1.0f));
        texturedShader.setVec3("viewPos", cameraPos);

        for (size_t i = 0; i < objects.size(); i++)
        {
            if (objects[i].isMoving)
            {
                glm::vec3 newPos = objects[i].trajectory.getNextPosition(deltaTime);
                glm::mat4 modelMatrix = objects[i].obj->getModelMatrix();
                glm::vec3 currentPos(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
                glm::vec3 translation = newPos - currentPos;
                objects[i].obj->translate(translation);
            }

            texturedShader.setMat4("model", objects[i].obj->getModelMatrix());

            if (i == selectedObject)
            {
                texturedShader.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 0.0f));
            }
            else
            {
                texturedShader.setVec3("objectColor", glm::vec3(0.8f, 0.8f, 0.8f));
            }

            if (objects[i].obj->hasTextures())
            {
                Material material = objects[i].obj->getMaterial();
                texturedShader.setVec3("material.ambient", material.ambient);
                texturedShader.setVec3("material.diffuse", material.diffuse);
                texturedShader.setVec3("material.specular", material.specular);
                texturedShader.setFloat("material.shininess", material.shininess);
                texturedShader.setBool("useTexture", true);

                objects[i].obj->drawTextured(texturedShader.ID);
            }
            else
            {
                texturedShader.setBool("useTexture", false);
                texturedShader.setVec3("material.ambient", glm::vec3(0.1f));
                texturedShader.setVec3("material.diffuse", glm::vec3(0.6f));
                texturedShader.setVec3("material.specular", glm::vec3(0.3f));
                texturedShader.setFloat("material.shininess", 16.0f);

                objects[i].obj->drawWithTextures();
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (const auto &obj : objects)
    {
        delete obj.obj;
    }
    glfwTerminate();
    return 0;
}