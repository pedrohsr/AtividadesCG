#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include "domain/Shader.hpp"
#include "domain/Obj.hpp"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

std::vector<Obj*> objects;
int selectedObject = 0;

enum TransformMode {
    TRANSLATE,
    ROTATE,
    SCALE
};
TransformMode currentMode = TRANSLATE;

bool wireframeMode = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    static bool tabPressed = false;
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        if (!tabPressed && !objects.empty()) {
            selectedObject = (selectedObject + 1) % objects.size();
            std::cout << "Selected object: " << selectedObject << std::endl;
            tabPressed = true;
        }
    } else {
        tabPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        currentMode = TRANSLATE;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        currentMode = ROTATE;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        currentMode = SCALE;

    static bool wKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (!wKeyPressed) {
            wireframeMode = !wireframeMode;
            wKeyPressed = true;
        }
    } else {
        wKeyPressed = false;
    }

    float speed = 2.5f * deltaTime;
    if (selectedObject >= 0 && selectedObject < objects.size()) {
        Obj* obj = objects[selectedObject];
        
        switch (currentMode) {
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

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <model1.obj> [model2.obj] [model3.obj] ...\n";
    std::cout << "You must provide at least one OBJ file path as argument.\n\n";
    std::cout << "Controls:\n";
    std::cout << "- TAB: Switch between objects\n";
    std::cout << "- T: Translation mode (use arrow keys and Page Up/Down)\n";
    std::cout << "- R: Rotation mode (use X, Y, Z keys)\n";
    std::cout << "- S: Scale mode (use Up/Down arrows)\n";
    std::cout << "- W: Toggle wireframe mode\n";
    std::cout << "- ESC: Exit\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return -1;
    }

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Model Viewer", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader shader("src/shaders/shader.vert", "src/shaders/shader.frag");

    float xOffset = 0.0f;
    for (int i = 1; i < argc; i++) {
        try {
            Obj* obj = new Obj(argv[i]);
            obj->translate(glm::vec3(xOffset, 0.0f, 0.0f));
            objects.push_back(obj);
            xOffset += 2.0f;
            std::cout << "Loaded model: " << argv[i] << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Failed to load model " << argv[i] << ": " << e.what() << std::endl;
        }
    }

    if (objects.empty()) {
        std::cerr << "No valid models were loaded. Exiting.\n";
        glfwTerminate();
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader.setMat4("view", view);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        for (size_t i = 0; i < objects.size(); i++) {
            shader.setMat4("model", objects[i]->getModelMatrix());
            
            if (i == selectedObject)
                shader.setVec3("objectColor", glm::vec3(1.0f, 1.0f, 0.0f));
            else
                shader.setVec3("objectColor", glm::vec3(0.8f, 0.8f, 0.8f));

            shader.setBool("wireframe", false);
            objects[i]->draw();

            if (wireframeMode) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                shader.setBool("wireframe", true);
                objects[i]->draw();
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (Obj* obj : objects) {
        delete obj;
    }
    glfwTerminate();
    return 0;
}
