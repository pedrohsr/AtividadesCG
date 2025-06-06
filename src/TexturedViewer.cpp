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

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void printUsage(const char *programName);

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

    float speed = 2.5f * deltaTime;
    if (selectedObject >= 0 && selectedObject < objects.size())
    {
        TexturedObj *obj = objects[selectedObject];

        switch (currentMode)
        {
        case TRANSLATE:
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                obj->translate(glm::vec3(0.0f, speed, 0.0f));
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                obj->translate(glm::vec3(0.0f, -speed, 0.0f));
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
                obj->translate(glm::vec3(-speed, 0.0f, 0.0f));
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                obj->translate(glm::vec3(speed, 0.0f, 0.0f));
            if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
                obj->translate(glm::vec3(0.0f, 0.0f, -speed));
            if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
                obj->translate(glm::vec3(0.0f, 0.0f, speed));
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
                obj->setScale(obj->scale + glm::vec3(speed));
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                obj->setScale(obj->scale - glm::vec3(speed));
            break;
        }
    }
}

void printUsage(const char *programName)
{
    std::cout << "=== TEXTURED VIEWER (Domain Classes) ===" << std::endl;
    std::cout << "Usage: " << programName << " <model1.obj> [model2.obj] [model3.obj] ..." << std::endl;
    std::cout << "Supports OBJ files with MTL materials and textures." << std::endl
              << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "- TAB: Switch between objects" << std::endl;
    std::cout << "- T: Translation mode (use arrow keys and Page Up/Down)" << std::endl;
    std::cout << "- R: Rotation mode (use X, Y, Z keys)" << std::endl;
    std::cout << "- G: Scale mode (use Up/Down arrows)" << std::endl;
    std::cout << "- W: Toggle wireframe mode" << std::endl;
    std::cout << "- ESC: Exit" << std::endl;
    std::cout << "=========================================" << std::endl;
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

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Textured Viewer (Domain)", NULL, NULL);
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

    Shader texturedShader("src/shaders/textured.vert", "src/shaders/textured.frag");

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
            texturedShader.setMat4("model", objects[i]->getModelMatrix());

            if (i == selectedObject)
            {
                texturedShader.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 0.0f));
            }
            else
            {
                texturedShader.setVec3("objectColor", glm::vec3(0.8f, 0.8f, 0.8f));
            }

            if (objects[i]->hasTextures())
            {
                Material material = objects[i]->getMaterial();
                texturedShader.setVec3("material.ambient", material.ambient);
                texturedShader.setVec3("material.diffuse", material.diffuse);
                texturedShader.setVec3("material.specular", material.specular);
                texturedShader.setFloat("material.shininess", material.shininess);
                texturedShader.setBool("useTexture", true);

                objects[i]->drawTextured(texturedShader.ID);
            }
            else
            {
                texturedShader.setBool("useTexture", false);
                texturedShader.setVec3("material.ambient", glm::vec3(0.1f));
                texturedShader.setVec3("material.diffuse", glm::vec3(0.6f));
                texturedShader.setVec3("material.specular", glm::vec3(0.3f));
                texturedShader.setFloat("material.shininess", 16.0f);

                objects[i]->drawWithTextures();
            }

            if (wireframeMode)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                texturedShader.setBool("useTexture", false);
                texturedShader.setVec3("objectColor", glm::vec3(1.0f, 0.0f, 0.0f));
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