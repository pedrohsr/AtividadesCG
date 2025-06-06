#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include "domain/Shader.hpp"
#include "domain/TexturedObj.hpp"

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct ThreePointLights
{
    glm::vec3 keyPos;
    glm::vec3 keyColor;
    float keyIntensity;
    bool keyEnabled;

    glm::vec3 fillPos;
    glm::vec3 fillColor;
    float fillIntensity;
    bool fillEnabled;

    glm::vec3 backPos;
    glm::vec3 backColor;
    float backIntensity;
    bool backEnabled;

    ThreePointLights()
    {
        keyColor = glm::vec3(1.0f, 1.0f, 0.9f);
        fillColor = glm::vec3(0.8f, 0.9f, 1.0f);
        backColor = glm::vec3(1.0f, 1.0f, 1.0f);

        keyIntensity = 1.0f;
        fillIntensity = 0.3f;
        backIntensity = 0.4f;

        keyEnabled = true;
        fillEnabled = true;
        backEnabled = true;
    }

    void setupLights(glm::vec3 objectPos, float objectScale)
    {
        float distance = objectScale * 4.0f;

        keyPos = objectPos + glm::vec3(distance * 0.7f, distance * 0.7f, distance * 0.7f);

        fillPos = objectPos + glm::vec3(-distance * 0.5f, distance * 0.3f, distance * 0.5f);

        backPos = objectPos + glm::vec3(0.0f, distance * 0.5f, -distance * 0.8f);
    }
};

ThreePointLights lights;
std::vector<TexturedObj *> objects;
int selectedObject = 0;

enum TransformMode
{
    TRANSLATE,
    ROTATE,
    SCALE
};
TransformMode currentMode = TRANSLATE;

bool wireframeMode = false;
bool showLightPositions = false;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void printUsage(const char *programName);
void updateLightPositions();

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    static bool tabPressed = false;
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
    {
        if (!tabPressed && !objects.empty())
        {
            selectedObject = (selectedObject + 1) % objects.size();
            std::cout << "Selected object: " << selectedObject << std::endl;
            updateLightPositions();
            tabPressed = true;
        }
    }
    else
    {
        tabPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        currentMode = TRANSLATE;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        currentMode = ROTATE;
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        currentMode = SCALE;

    static bool wKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if (!wKeyPressed)
        {
            wireframeMode = !wireframeMode;
            wKeyPressed = true;
        }
    }
    else
    {
        wKeyPressed = false;
    }

    static bool key1Pressed = false, key2Pressed = false, key3Pressed = false;

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        if (!key1Pressed)
        {
            lights.keyEnabled = !lights.keyEnabled;
            std::cout << "Key Light: " << (lights.keyEnabled ? "ON" : "OFF") << std::endl;
            key1Pressed = true;
        }
    }
    else
    {
        key1Pressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        if (!key2Pressed)
        {
            lights.fillEnabled = !lights.fillEnabled;
            std::cout << "Fill Light: " << (lights.fillEnabled ? "ON" : "OFF") << std::endl;
            key2Pressed = true;
        }
    }
    else
    {
        key2Pressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        if (!key3Pressed)
        {
            lights.backEnabled = !lights.backEnabled;
            std::cout << "Back Light: " << (lights.backEnabled ? "ON" : "OFF") << std::endl;
            key3Pressed = true;
        }
    }
    else
    {
        key3Pressed = false;
    }

    float speed = 2.5f * deltaTime;
    if (selectedObject >= 0 && selectedObject < objects.size())
    {
        TexturedObj *obj = objects[selectedObject];
        bool objectMoved = false;

        switch (currentMode)
        {
        case TRANSLATE:
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            {
                obj->translate(glm::vec3(0.0f, speed, 0.0f));
                objectMoved = true;
            }
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            {
                obj->translate(glm::vec3(0.0f, -speed, 0.0f));
                objectMoved = true;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            {
                obj->translate(glm::vec3(-speed, 0.0f, 0.0f));
                objectMoved = true;
            }
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            {
                obj->translate(glm::vec3(speed, 0.0f, 0.0f));
                objectMoved = true;
            }
            if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
            {
                obj->translate(glm::vec3(0.0f, 0.0f, -speed));
                objectMoved = true;
            }
            if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
            {
                obj->translate(glm::vec3(0.0f, 0.0f, speed));
                objectMoved = true;
            }
            break;

        case ROTATE:
            if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
                obj->rotate(speed * 50.0f, glm::vec3(1.0f, 0.0f, 0.0f));
            if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
                obj->rotate(speed * 50.0f, glm::vec3(0.0f, 1.0f, 0.0f));
            if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
                obj->rotate(speed * 50.0f, glm::vec3(0.0f, 0.0f, 1.0f));
            break;

        case SCALE:
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            {
                obj->setScale(obj->scale + glm::vec3(speed));
                objectMoved = true;
            }
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            {
                obj->setScale(obj->scale - glm::vec3(speed));
                objectMoved = true;
            }
            break;
        }

        if (objectMoved)
        {
            updateLightPositions();
        }
    }
}

void updateLightPositions()
{
    if (!objects.empty() && selectedObject >= 0 && selectedObject < objects.size())
    {
        TexturedObj *obj = objects[selectedObject];
        float maxScale = std::max(std::max(obj->scale.x, obj->scale.y), obj->scale.z);

        glm::mat4 modelMatrix = obj->getModelMatrix();
        glm::vec3 objectPos = glm::vec3(modelMatrix[3]);

        lights.setupLights(objectPos, maxScale);

        if (showLightPositions)
        {
            std::cout << "Lights updated for object at (" << objectPos.x << ", " << objectPos.y << ", " << objectPos.z << ")" << std::endl;
        }
    }
}

void printUsage(const char *programName)
{
    std::cout << "=== THREE POINT LIGHTING SYSTEM ===" << std::endl;
    std::cout << "Usage: " << programName << " <model1.obj> [model2.obj] [model3.obj] ..." << std::endl;
    std::cout << "Implements classic three-point lighting technique." << std::endl
              << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "- TAB: Switch between objects (updates lighting)" << std::endl;
    std::cout << "- T: Translation mode (arrow keys + Page Up/Down)" << std::endl;
    std::cout << "- R: Rotation mode (X, Y, Z keys)" << std::endl;
    std::cout << "- G: Scale mode (Up/Down arrows)" << std::endl;
    std::cout << "- W: Toggle wireframe mode" << std::endl;
    std::cout << "Three Point Light Controls:" << std::endl;
    std::cout << "- 1: Toggle Key Light (main light)" << std::endl;
    std::cout << "- 2: Toggle Fill Light (shadow softener)" << std::endl;
    std::cout << "- 3: Toggle Back Light (background separator)" << std::endl;
    std::cout << "- ESC: Exit" << std::endl;
    std::cout << "====================================" << std::endl;
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

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Three Point Lighting System", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader threePointShader("src/shaders/three_point.vert", "src/shaders/three_point.frag");

    float xOffset = 0.0f;
    for (int i = 1; i < argc; i++)
    {
        try
        {
            TexturedObj *obj = new TexturedObj(argv[i]);
            obj->translate(glm::vec3(xOffset, 0.0f, 0.0f));
            objects.push_back(obj);
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

    updateLightPositions();

    std::cout << "\nThree Point Lighting System initialized!" << std::endl;
    std::cout << "Key Light: ON, Fill Light: ON, Back Light: ON" << std::endl;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        threePointShader.use();

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        threePointShader.setMat4("view", view);
        threePointShader.setMat4("projection", projection);
        threePointShader.setVec3("viewPos", cameraPos);

        threePointShader.setBool("keyEnabled", lights.keyEnabled);
        threePointShader.setBool("fillEnabled", lights.fillEnabled);
        threePointShader.setBool("backEnabled", lights.backEnabled);

        threePointShader.setVec3("keyPos", lights.keyPos);
        threePointShader.setVec3("keyColor", lights.keyColor);
        threePointShader.setFloat("keyIntensity", lights.keyIntensity);

        threePointShader.setVec3("fillPos", lights.fillPos);
        threePointShader.setVec3("fillColor", lights.fillColor);
        threePointShader.setFloat("fillIntensity", lights.fillIntensity);

        threePointShader.setVec3("backPos", lights.backPos);
        threePointShader.setVec3("backColor", lights.backColor);
        threePointShader.setFloat("backIntensity", lights.backIntensity);

        for (size_t i = 0; i < objects.size(); i++)
        {
            threePointShader.setMat4("model", objects[i]->getModelMatrix());

            bool isSelected = (i == selectedObject);
            threePointShader.setBool("isSelected", isSelected);

            if (objects[i]->hasMaterials())
            {
                Material material = objects[i]->getMaterial();
                threePointShader.setFloat("ka", material.ambient.x);
                threePointShader.setFloat("kd", material.diffuse.x);
                threePointShader.setFloat("ks", material.specular.x);
                threePointShader.setFloat("q", material.shininess);

                if (objects[i]->hasTextures())
                {
                    threePointShader.setBool("useTexture", true);
                    objects[i]->drawTextured(threePointShader.ID);
                }
                else
                {
                    threePointShader.setBool("useTexture", false);
                    objects[i]->drawWithTextures();
                }
            }
            else
            {
                threePointShader.setFloat("ka", 0.1f);
                threePointShader.setFloat("kd", 0.7f);
                threePointShader.setFloat("ks", 0.3f);
                threePointShader.setFloat("q", 32.0f);
                threePointShader.setBool("useTexture", false);

                objects[i]->drawWithTextures();
            }

            if (wireframeMode)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                threePointShader.setBool("useTexture", false);
                threePointShader.setFloat("kd", 1.0f);
                objects[i]->drawWithTextures();
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (TexturedObj *obj : objects)
    {
        delete obj;
    }
    glfwTerminate();
    return 0;
}