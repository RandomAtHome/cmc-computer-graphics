#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include "GL/freeglut.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <soil.h>
#include <iostream>

// GLEW VERSION IS 4.0

bool pressedKeys[1024];
GLfloat frameDeltaTime = 0.0f;
GLfloat lastFrameTime = 0.0; 
bool isFlashlightOn = false;
double mousePrevX, mousePrevY;
Camera mainCamera(glm::vec3(0.0f, 0.0f, 3.0f));
const unsigned int screenWidth = 800;
const unsigned int screenHeight = 600;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (action == GLFW_PRESS) {
        pressedKeys[key] = true;
    } else if (action == GLFW_RELEASE) {
        pressedKeys[key] = false;
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    float xoffset = xpos - mousePrevX;
    float yoffset = mousePrevY - ypos; // reversed since y-coordinates go from bottom to top
    mainCamera.ProcessMouseMovement(xoffset, yoffset);
    mousePrevX = xpos;
    mousePrevY = ypos;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void processCameraMovement(Camera &camera) {
    if (pressedKeys[GLFW_KEY_SPACE]) {
        camera.ProcessKeyboard(UP, frameDeltaTime);
    }
    if (pressedKeys[GLFW_KEY_X]) {
        camera.ProcessKeyboard(DOWN, frameDeltaTime);
    }
    if (pressedKeys[GLFW_KEY_W]) {
        camera.ProcessKeyboard(FORWARD, frameDeltaTime);
    }
    if (pressedKeys[GLFW_KEY_A]) {
        camera.ProcessKeyboard(LEFT, frameDeltaTime);
    }
    if (pressedKeys[GLFW_KEY_S]) {
        camera.ProcessKeyboard(BACKWARD, frameDeltaTime);
    }
    if (pressedKeys[GLFW_KEY_D]) {
        camera.ProcessKeyboard(RIGHT, frameDeltaTime);
    }
}

void processActionKeys() {
    if (pressedKeys[GLFW_KEY_F]) {
        isFlashlightOn ^= 1;
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Learn3DOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetCursorPos(window, &mousePrevX, &mousePrevY);


    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //////////////////////////////////Verticies
    Shader ourShader("Shaders/crysis_model.vert", "Shaders/crysis_model.frag");
    Model ourModel("Objects/Crysis_Model/nanosuit.obj");
    //////////////////////////////////Pre-loop configs

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrameTime = glfwGetTime();
        frameDeltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;
        
        processCameraMovement(mainCamera);
        processActionKeys();

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(mainCamera.Zoom), (GLfloat)screenWidth / screenHeight, 0.1f, 100.0f);
        glm::mat4 view = mainCamera.GetViewMatrix();
        ourShader.Use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        // render the loaded model
        glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}