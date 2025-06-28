#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include "domain/Shader.hpp"
#include "domain/TexturedObj.hpp"
#include "domain/Trajectory.hpp"
#include "domain/Camera.hpp"

using json = nlohmann::json;

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

unsigned int windowWidth = SCR_WIDTH;
unsigned int windowHeight = SCR_HEIGHT;

Camera camera(glm::vec3(0.0f, 2.0f, 8.0f));

struct Light {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    bool enabled;
};

std::vector<Light> lights;
int selectedLight = 0;

struct SceneObject {
    TexturedObj* obj;
    Trajectory trajectory;
    bool isMoving;
    std::string name;
    std::string file;
    glm::vec3 initialPosition;
    glm::vec3 initialRotation;
    glm::vec3 initialScale;
};

std::vector<SceneObject> sceneObjects;
int selectedObject = 0;

bool wireframeMode = false;

enum TransformMode {
    TRANSLATE,
    ROTATE,
    SCALE
};
TransformMode currentMode = TRANSLATE;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
void printUsage(const char* programName);
bool loadSceneConfig(const std::string& filename);
void saveSceneConfig(const std::string& filename);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    windowWidth = width;
    windowHeight = height;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.Rotate(xoffset, yoffset);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.MoveForward(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.MoveBackward(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.MoveLeft(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.MoveRight(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.MoveUp(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.MoveDown(deltaTime);

    static bool tabPressed = false;
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        if (!tabPressed && !sceneObjects.empty()) {
            selectedObject = (selectedObject + 1) % sceneObjects.size();
            std::cout << "Selected object: " << sceneObjects[selectedObject].name << std::endl;
            tabPressed = true;
        }
    } else {
        tabPressed = false;
    }

    static bool lPressed = false;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        if (!lPressed && !lights.empty()) {
            selectedLight = (selectedLight + 1) % lights.size();
            std::cout << "Selected light: " << selectedLight << std::endl;
            lPressed = true;
        }
    } else {
        lPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        currentMode = TRANSLATE;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        currentMode = ROTATE;
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        currentMode = SCALE;

    static bool fKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        if (!fKeyPressed) {
            wireframeMode = !wireframeMode;
            fKeyPressed = true;
        }
    } else {
        fKeyPressed = false;
    }

    if (!sceneObjects.empty() && selectedObject >= 0 && selectedObject < sceneObjects.size()) {
        SceneObject& obj = sceneObjects[selectedObject];
        float speed = 2.5f * deltaTime;

        static bool cPressed = false;
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            if (!cPressed) {
                glm::mat4 modelMatrix = obj.obj->getModelMatrix();
                glm::vec3 currentPos(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
                obj.trajectory.addPoint(currentPos);
                cPressed = true;
            }
        } else {
            cPressed = false;
        }

        static bool mPressed = false;
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
            if (!mPressed) {
                obj.isMoving = !obj.isMoving;
                std::cout << "Object " << obj.name << (obj.isMoving ? " started" : " stopped") << " movement" << std::endl;
                mPressed = true;
            }
        } else {
            mPressed = false;
        }

        static bool nPressed = false;
        if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
            if (!nPressed) {
                obj.trajectory.clear();
                std::cout << "Cleared trajectory for " << obj.name << std::endl;
                nPressed = true;
            }
        } else {
            nPressed = false;
        }

        static bool iPressed = false;
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
            if (!iPressed) {
                InterpolationType currentType = obj.trajectory.getInterpolationType();
                InterpolationType newType;
                switch (currentType) {
                    case InterpolationType::LINEAR:
                        newType = InterpolationType::BEZIER;
                        break;
                    case InterpolationType::BEZIER:
                        newType = InterpolationType::SPLINE;
                        break;
                    case InterpolationType::SPLINE:
                        newType = InterpolationType::LINEAR;
                        break;
                }
                obj.trajectory.setInterpolationType(newType);
                std::cout << "Changed interpolation type for " << obj.name << std::endl;
                iPressed = true;
            }
        } else {
            iPressed = false;
        }

        static bool oPressed = false;
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
            if (!oPressed) {
                InterpolationType currentType = obj.trajectory.getInterpolationType();
                std::string typeName;
                switch (currentType) {
                    case InterpolationType::LINEAR:
                        typeName = "LINEAR";
                        break;
                    case InterpolationType::BEZIER:
                        typeName = "BEZIER";
                        break;
                    case InterpolationType::SPLINE:
                        typeName = "SPLINE";
                        break;
                }
                std::cout << "Object " << obj.name << " interpolation type: " << typeName 
                          << " (Speed: " << obj.trajectory.getSpeed() << ")" << std::endl;
                oPressed = true;
            }
        } else {
            oPressed = false;
        }

        static bool pPressed = false;
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            if (!pPressed) {
                float currentSpeed = obj.trajectory.getSpeed();
                float newSpeed = currentSpeed + 0.5f;
                if (newSpeed > 5.0f) newSpeed = 0.5f;
                obj.trajectory.setSpeed(newSpeed);
                std::cout << "Changed trajectory speed for " << obj.name << " to: " << newSpeed << std::endl;
                pPressed = true;
            }
        } else {
            pPressed = false;
        }

        switch (currentMode) {
            case TRANSLATE:
                if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                    obj.obj->translate(glm::vec3(0.0f, speed, 0.0f));
                if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                    obj.obj->translate(glm::vec3(0.0f, -speed, 0.0f));
                if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
                    obj.obj->translate(glm::vec3(-speed, 0.0f, 0.0f));
                if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                    obj.obj->translate(glm::vec3(speed, 0.0f, 0.0f));
                if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
                    obj.obj->translate(glm::vec3(0.0f, 0.0f, -speed));
                if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
                    obj.obj->translate(glm::vec3(0.0f, 0.0f, speed));
                break;

            case ROTATE:
                if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
                    obj.obj->rotate(speed * 50.0f, glm::vec3(1.0f, 0.0f, 0.0f));
                if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
                    obj.obj->rotate(speed * 50.0f, glm::vec3(0.0f, 1.0f, 0.0f));
                if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
                    obj.obj->rotate(speed * 50.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                break;

            case SCALE:
                if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                    obj.obj->setScale(obj.obj->scale + glm::vec3(speed * 0.1f));
                if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                    glm::vec3 newScale = obj.obj->scale - glm::vec3(speed * 0.1f);
                    newScale = glm::max(newScale, glm::vec3(0.01f)); // Minimum scale of 0.01
                    obj.obj->setScale(newScale);
                }
                break;
        }
    }

    if (!lights.empty() && selectedLight >= 0 && selectedLight < lights.size()) {
        Light& light = lights[selectedLight];
        float lightSpeed = 3.0f * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
            light.position.x -= lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
            light.position.x += lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
            light.position.y -= lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
            light.position.y += lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS)
            light.position.z -= lightSpeed;
        if (glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS)
            light.position.z += lightSpeed;

        if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
            light.intensity += 0.1f;
        if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
            light.intensity = std::max(0.0f, light.intensity - 0.1f);
    }

    static bool f1Pressed = false;
    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
        if (!f1Pressed) {
            saveSceneConfig("scene_config.json");
            f1Pressed = true;
        }
    } else {
        f1Pressed = false;
    }

    static bool f2Pressed = false;
    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
        if (!f2Pressed) {
            loadSceneConfig("scene_config.json");
            f2Pressed = true;
        }
    } else {
        f2Pressed = false;
    }
}

void printUsage(const char* programName) {
    std::cout << "=== SCENE VIEWER - Complete 3D Scene Visualization ===" << std::endl;
    std::cout << "Usage: " << programName << " [scene_config.json]" << std::endl;
    std::cout << "Features: Multiple OBJ loading, Phong lighting, parametric curves, JSON scene config" << std::endl;
    std::cout << std::endl;
    std::cout << "Camera Controls:" << std::endl;
    std::cout << "- WASD: Move camera" << std::endl;
    std::cout << "- Mouse: Look around" << std::endl;
    std::cout << "- Space/Ctrl: Move up/down" << std::endl;
    std::cout << std::endl;
    std::cout << "Object Controls:" << std::endl;
    std::cout << "- TAB: Switch between objects" << std::endl;
    std::cout << "- T/R/G: Translation/Rotation/Scale mode" << std::endl;
    std::cout << "- Z/X/Y: Rotate selected object" << std::endl;
    std::cout << "- Arrow keys: Transform selected object" << std::endl;
    std::cout << "- Page Up/Down: Move object forward/backward" << std::endl;
    std::cout << "- C: Add trajectory point to selected object" << std::endl;
    std::cout << "- M: Toggle trajectory movement" << std::endl;
    std::cout << "- N: Clear trajectory" << std::endl;
    std::cout << "- I: Change interpolation type (Linear -> Bezier -> Spline)" << std::endl;
    std::cout << "- O: Display current interpolation type and speed" << std::endl;
    std::cout << "- P: Cycle trajectory speed (0.5 -> 1.0 -> 1.5 -> ... -> 5.0)" << std::endl;
    std::cout << std::endl;
    std::cout << "Parametric Curves:" << std::endl;
    std::cout << "- Linear: Straight lines between control points" << std::endl;
    std::cout << "- Bezier: Smooth curves using all control points" << std::endl;
    std::cout << "- Spline: Smooth curves with local control (Catmull-Rom)" << std::endl;
    std::cout << "  * Linear: Best for simple paths, predictable movement" << std::endl;
    std::cout << "  * Bezier: Best for smooth, organic curves" << std::endl;
    std::cout << "  * Spline: Best for complex paths with local control" << std::endl;
    std::cout << std::endl;
    std::cout << "Light Controls:" << std::endl;
    std::cout << "- L: Switch between lights" << std::endl;
    std::cout << "- Numpad 4/6: Move light left/right" << std::endl;
    std::cout << "- Numpad 2/8: Move light down/up" << std::endl;
    std::cout << "- Numpad 7/9: Move light forward/backward" << std::endl;
    std::cout << "- Numpad +/-: Adjust light intensity" << std::endl;
    std::cout << std::endl;
    std::cout << "Display Controls:" << std::endl;
    std::cout << "- F: Toggle wireframe mode" << std::endl;
    std::cout << std::endl;
    std::cout << "Viewer Operations:" << std::endl;
    std::cout << "- F1: Save scene configuration (JSON)" << std::endl;
    std::cout << "- F2: Load scene configuration (JSON)" << std::endl;
    std::cout << "- ESC: Exit" << std::endl;
    std::cout << "==================================================" << std::endl;
}

bool loadSceneConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "No scene config found, scene not loaded" << std::endl;
        return false;
    }

    try {
        json sceneData;
        file >> sceneData;
        file.close();

        std::cout << "Loading scene configuration from: " << filename << std::endl;
        
        for (auto& obj : sceneObjects) {
            delete obj.obj;
        }
        sceneObjects.clear();
        lights.clear();

        if (sceneData.contains("camera")) {
            auto cam = sceneData["camera"];
            glm::vec3 pos(cam["position"][0], cam["position"][1], cam["position"][2]);
            float yaw = cam.value("yaw", -90.0f);
            float pitch = cam.value("pitch", 0.0f);
            camera = Camera(pos, glm::vec3(0.0f, 1.0f, 0.0f), yaw, pitch);
            std::cout << "Set camera position to (" << pos.x << ", " << pos.y << ", " << pos.z << ") with yaw=" << yaw << ", pitch=" << pitch << std::endl;
        }

        if (sceneData.contains("lights")) {
            for (const auto& lightData : sceneData["lights"]) {
                Light light;
                light.position = glm::vec3(lightData["position"][0], lightData["position"][1], lightData["position"][2]);
                light.color = glm::vec3(lightData["color"][0], lightData["color"][1], lightData["color"][2]);
                light.intensity = lightData["intensity"];
                light.enabled = lightData.value("enabled", true);
                lights.push_back(light);
                std::cout << "Added light at (" << light.position.x << ", " << light.position.y << ", " << light.position.z << ")" << std::endl;
            }
        }

        if (sceneData.contains("objects")) {
            for (const auto& objData : sceneData["objects"]) {
                std::string objFile = objData["file"];
                std::string objName = objData["name"];
                
                try {
                    TexturedObj* obj = new TexturedObj(objFile);
                    
                    if (objData.contains("position")) {
                        glm::vec3 pos(objData["position"][0], objData["position"][1], objData["position"][2]);
                        obj->translate(pos);
                    }
                    
                    if (objData.contains("rotation")) {
                        glm::vec3 rot(objData["rotation"][0], objData["rotation"][1], objData["rotation"][2]);
                        std::cout << "Applying rotation to " << objName << ": (" << rot.x << ", " << rot.y << ", " << rot.z << ")" << std::endl;
                        obj->setRotation(rot);
                        std::cout << "  Set rotation to: (" << rot.x << ", " << rot.y << ", " << rot.z << ") degrees" << std::endl;
                    }
                    
                    if (objData.contains("scale")) {
                        glm::vec3 scale(objData["scale"][0], objData["scale"][1], objData["scale"][2]);
                        obj->setScale(scale);
                    }

                    glm::vec3 initialPos = objData.contains("position") ? 
                        glm::vec3(objData["position"][0], objData["position"][1], objData["position"][2]) : glm::vec3(0.0f);
                    glm::vec3 initialRot = objData.contains("rotation") ? 
                        glm::vec3(objData["rotation"][0], objData["rotation"][1], objData["rotation"][2]) : glm::vec3(0.0f);
                    glm::vec3 initialScale = objData.contains("scale") ? 
                        glm::vec3(objData["scale"][0], objData["scale"][1], objData["scale"][2]) : glm::vec3(1.0f);

                    SceneObject sceneObj = {obj, Trajectory(), false, objName, objFile, initialPos, initialRot, initialScale};
                    
                    if (objData.contains("trajectory")) {
                        auto trajData = objData["trajectory"];
                        if (trajData.contains("points")) {
                            for (const auto& point : trajData["points"]) {
                                glm::vec3 trajPoint(point[0], point[1], point[2]);
                                sceneObj.trajectory.addPoint(trajPoint);
                            }
                        }
                        if (trajData.contains("speed")) {
                            sceneObj.trajectory.setSpeed(trajData["speed"]);
                        }
                        if (trajData.contains("autoStart")) {
                            sceneObj.isMoving = trajData["autoStart"];
                        }
                        if (trajData.contains("interpolationType")) {
                            std::string interpType = trajData["interpolationType"];
                            if (interpType == "LINEAR") {
                                sceneObj.trajectory.setInterpolationType(InterpolationType::LINEAR);
                            } else if (interpType == "BEZIER") {
                                sceneObj.trajectory.setInterpolationType(InterpolationType::BEZIER);
                            } else if (interpType == "SPLINE") {
                                sceneObj.trajectory.setInterpolationType(InterpolationType::SPLINE);
                            }
                        }
                    }
                    
                    sceneObjects.push_back(sceneObj);
                    std::cout << "Loaded object: " << objName << " from " << objFile << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "Failed to load object " << objFile << ": " << e.what() << std::endl;
                }
            }
        }

        return true;
    } catch (const json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return false;
    }
}

void saveSceneConfig(const std::string& filename) {
    try {
        json sceneData;

        glm::vec3 camPos = camera.GetPosition();
        sceneData["camera"]["position"] = {camPos.x, camPos.y, camPos.z};
        sceneData["camera"]["yaw"] = camera.GetYaw();
        sceneData["camera"]["pitch"] = camera.GetPitch();

        sceneData["lights"] = json::array();
        for (const auto& light : lights) {
            json lightData;
            lightData["position"] = {light.position.x, light.position.y, light.position.z};
            lightData["color"] = {light.color.x, light.color.y, light.color.z};
            lightData["intensity"] = light.intensity;
            lightData["enabled"] = light.enabled;
            sceneData["lights"].push_back(lightData);
        }

        sceneData["objects"] = json::array();
        for (const auto& obj : sceneObjects) {
            json objData;
            objData["name"] = obj.name;
            objData["file"] = obj.file;
            
            glm::mat4 modelMatrix = obj.obj->getModelMatrix();
            glm::vec3 currentPos(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
            objData["position"] = {currentPos.x, currentPos.y, currentPos.z};
            
            glm::vec3 currentRotation = obj.obj->getRotation();
            objData["rotation"] = {currentRotation.x, currentRotation.y, currentRotation.z};
            objData["scale"] = {obj.obj->scale.x, obj.obj->scale.y, obj.obj->scale.z};

            const auto& trajPoints = obj.trajectory.getControlPoints();
            if (!trajPoints.empty()) {
                json trajData;
                trajData["points"] = json::array();
                for (const auto& point : trajPoints) {
                    trajData["points"].push_back({point.x, point.y, point.z});
                }
                trajData["speed"] = obj.trajectory.getSpeed();
                trajData["autoStart"] = obj.isMoving;
                trajData["interpolationType"] = obj.trajectory.getInterpolationType() == InterpolationType::LINEAR ? "LINEAR" :
                    obj.trajectory.getInterpolationType() == InterpolationType::BEZIER ? "BEZIER" : "SPLINE";
                objData["trajectory"] = trajData;
            }

            sceneData["objects"].push_back(objData);
        }

        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to save scene configuration" << std::endl;
            return;
        }

        file << sceneData.dump(2);
        file.close();
        
        std::cout << "Scene configuration saved successfully to: " << filename << std::endl;
    } catch (const json::exception& e) {
        std::cerr << "JSON error while saving: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    printUsage(argv[0]);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Scene Viewer - Complete 3D Visualization", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader phongShader("src/shaders/phong.vert", "src/shaders/phong.frag");

    bool configLoaded = false;
    if (argc > 1) {
        configLoaded = loadSceneConfig(argv[1]);
    }
    if (!configLoaded) {
        configLoaded = loadSceneConfig("scene_config.json");
    }

    if (!configLoaded) {
        std::cerr << "ERROR: No scene configuration found!" << std::endl;
        std::cerr << "Please provide a scene configuration file:" << std::endl;
        std::cerr << "  " << argv[0] << " scene_config.json" << std::endl;
        std::cerr << "Or create a scene_config.json file in the current directory." << std::endl;
        glfwTerminate();
        return -1;
    }

    if (sceneObjects.empty()) {
        std::cerr << "ERROR: No objects found in scene configuration!" << std::endl;
        std::cerr << "Please add objects to your scene_config.json file." << std::endl;
        glfwTerminate();
        return -1;
    } else {
        std::cout << "Scene loaded with " << sceneObjects.size() << " objects and " << lights.size() << " lights." << std::endl;
    }

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        phongShader.use();

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

        phongShader.setMat4("view", view);
        phongShader.setMat4("projection", projection);
        phongShader.setVec3("viewPos", camera.GetPosition());

        if (!lights.empty()) {
            phongShader.setVec3("lightPos", lights[0].position);
            phongShader.setVec3("lightColor", lights[0].color * lights[0].intensity);
            
            if (lights.size() > 1) {
                glm::vec3 totalLightColor = lights[0].color * lights[0].intensity;
                for (size_t i = 1; i < lights.size(); i++) {
                    if (lights[i].enabled) {
                        totalLightColor += lights[i].color * lights[i].intensity;
                    }
                }
                phongShader.setVec3("lightColor", totalLightColor);
            }
        } else {
            phongShader.setVec3("lightPos", glm::vec3(2.0f, 4.0f, 6.0f));
            phongShader.setVec3("lightColor", glm::vec3(1.0f));
        }

        for (auto& obj : sceneObjects) {
            if (obj.isMoving) {
                glm::vec3 newPos = obj.trajectory.getNextPosition(deltaTime);
                glm::mat4 modelMatrix = obj.obj->getModelMatrix();
                glm::vec3 currentPos(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
                glm::vec3 translation = newPos - currentPos;
                obj.obj->translate(translation);
            }
        }

        for (size_t i = 0; i < sceneObjects.size(); i++) {
            const auto& obj = sceneObjects[i];
            
            glm::mat4 modelMatrix = obj.obj->getModelMatrix();
            phongShader.setMat4("model", modelMatrix);
            phongShader.setBool("isSelected", (i == selectedObject));

            if (obj.obj->hasMaterials()) {
                Material material = obj.obj->getMaterial();
                phongShader.setFloat("ka", material.ambient.x);
                phongShader.setFloat("kd", material.diffuse.x);
                phongShader.setFloat("ks", material.specular.x);
                phongShader.setFloat("q", material.shininess);

                if (obj.obj->hasTextures()) {
                    phongShader.setBool("useTexture", true);
                    obj.obj->drawTextured(phongShader.ID);
                } else {
                    phongShader.setBool("useTexture", false);
                    obj.obj->drawWithTextures();
                }
            } else {
                phongShader.setFloat("ka", 0.1f);
                phongShader.setFloat("kd", 0.5f);
                phongShader.setFloat("ks", 0.5f);
                phongShader.setFloat("q", 10.0f);
                phongShader.setBool("useTexture", false);
                obj.obj->drawWithTextures();
            }

            if (wireframeMode) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                phongShader.setBool("useTexture", false);
                phongShader.setFloat("kd", 1.0f);
                obj.obj->drawWithTextures();
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (auto& obj : sceneObjects) {
        delete obj.obj;
    }

    glfwTerminate();
    return 0;
} 