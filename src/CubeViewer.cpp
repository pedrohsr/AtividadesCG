#include <iostream>
#include <string>
#include <assert.h>
#include <vector>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Cube
{
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;

    Cube(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 scl = glm::vec3(1.0f))
        : position(pos), scale(scl), rotation(0.0f) {}
};

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

int setupShader();
int setupGeometry();
glm::mat4 getModelMatrix(const Cube &cube);

const GLuint WIDTH = 1200, HEIGHT = 800;

const GLchar *vertexShaderSource = "#version 450\n"
                                   "layout (location = 0) in vec3 position;\n"
                                   "layout (location = 1) in vec3 color;\n"
                                   "uniform mat4 model;\n"
                                   "uniform mat4 view;\n"
                                   "uniform mat4 projection;\n"
                                   "out vec4 finalColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "gl_Position = projection * view * model * vec4(position, 1.0);\n"
                                   "finalColor = vec4(color, 1.0);\n"
                                   "}\0";

const GLchar *fragmentShaderSource = "#version 450\n"
                                     "in vec4 finalColor;\n"
                                     "out vec4 color;\n"
                                     "void main()\n"
                                     "{\n"
                                     "color = finalColor;\n"
                                     "}\n\0";

vector<Cube> cubes;
int selectedCube = 0;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool keys[1024] = {false};

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 8.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void processMovement();

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Cube Viewer 3D -- Pedro Rodrigues!", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    GLuint shaderID = setupShader();
    GLuint VAO = setupGeometry();

    cubes.push_back(Cube(glm::vec3(-2.0f, 0.0f, 0.0f)));
    cubes.push_back(Cube(glm::vec3(2.0f, 0.0f, 0.0f)));
    cubes.push_back(Cube(glm::vec3(0.0f, 2.0f, 0.0f)));

    glUseProgram(shaderID);
    glEnable(GL_DEPTH_TEST);

    GLint modelLoc = glGetUniformLocation(shaderID, "model");
    GLint viewLoc = glGetUniformLocation(shaderID, "view");
    GLint projectionLoc = glGetUniformLocation(shaderID, "projection");

    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                            (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    cout << "\n=== CONTROLES ===" << endl;
    cout << "X, Y, Z: Rotação manual nos eixos (mantenha pressionado)" << endl;
    cout << "W, A, S, D: Mover nos eixos X e Z" << endl;
    cout << "I, J: Mover no eixo Y (cima/baixo)" << endl;
    cout << "Q, E: Escala (diminuir/aumentar)" << endl;
    cout << "TAB: Alternar entre cubos" << endl;
    cout << "SPACE: Adicionar novo cubo" << endl;
    cout << "R: Resetar cubo selecionado" << endl;
    cout << "ESC: Sair" << endl;
    cout << "=================" << endl;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        processMovement();

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glBindVertexArray(VAO);

        for (size_t i = 0; i < cubes.size(); i++)
        {
            glm::mat4 model = getModelMatrix(cubes[i]);

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            if (i == selectedCube)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glLineWidth(3.0f);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
    {
        if (!cubes.empty())
        {
            selectedCube = (selectedCube + 1) % cubes.size();
            cout << "Cubo selecionado: " << selectedCube << endl;
        }
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        float x = (rand() % 10 - 5) * 0.5f;
        float y = (rand() % 10 - 5) * 0.5f;
        float z = (rand() % 10 - 5) * 0.5f;
        cubes.push_back(Cube(glm::vec3(x, y, z)));
        cout << "Novo cubo adicionado. Total: " << cubes.size() << endl;
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        if (!cubes.empty())
        {
            cubes[selectedCube].position = glm::vec3(0.0f);
            cubes[selectedCube].scale = glm::vec3(1.0f);
            cubes[selectedCube].rotation = glm::vec3(0.0f);
            cout << "Cubo " << selectedCube << " resetado" << endl;
        }
    }
}

glm::mat4 getModelMatrix(const Cube &cube)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, cube.position);
    model = glm::rotate(model, cube.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, cube.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, cube.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, cube.scale);
    return model;
}

int setupShader()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int setupGeometry()
{
    GLfloat vertices[] = {
        // Face frontal (vermelha)
        -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

        // Face traseira (verde)
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,

        // Face esquerda (azul)
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f,

        // Face direita (amarela)
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,

        // Face superior (magenta)
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f,

        // Face inferior (ciano)
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f};

    GLuint VBO, VAO;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

void processMovement()
{
    if (cubes.empty() || selectedCube >= cubes.size())
        return;

    Cube &cube = cubes[selectedCube];
    float speed = 2.5f * deltaTime;

    if (keys[GLFW_KEY_W])
        cube.position.z -= speed;
    if (keys[GLFW_KEY_S])
        cube.position.z += speed;
    if (keys[GLFW_KEY_A])
        cube.position.x -= speed;
    if (keys[GLFW_KEY_D])
        cube.position.x += speed;

    if (keys[GLFW_KEY_I])
        cube.position.y += speed;
    if (keys[GLFW_KEY_J])
        cube.position.y -= speed;

    if (keys[GLFW_KEY_X])
        cube.rotation.x += speed * 0.5f;
    if (keys[GLFW_KEY_Y])
        cube.rotation.y += speed * 0.5f;
    if (keys[GLFW_KEY_Z])
        cube.rotation.z += speed * 0.5f;

    if (keys[GLFW_KEY_Q])
    {
        cube.scale *= (1.0f - speed * 0.5f);
        if (cube.scale.x < 0.1f)
            cube.scale = glm::vec3(0.1f);
    }
    if (keys[GLFW_KEY_E])
    {
        cube.scale *= (1.0f + speed * 0.5f);
        if (cube.scale.x > 3.0f)
            cube.scale = glm::vec3(3.0f);
    }
}