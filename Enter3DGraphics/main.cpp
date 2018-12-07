#include "Shader.h"
#include "Camera.h"
#include "cube_vertices.h"

#include "GL/glew.h"
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
double mousePrevX, mousePrevY;

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

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {}

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
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glViewport(0, 0, screenWidth, screenHeight);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetCursorPos(window, &mousePrevX, &mousePrevY);

    //////////////////////////////////Verticies
    //extern float *cube_vertices;
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    //////////////////////////////////Buffers
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    // Атрибут с координатами
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO

    ///////////////////////////////////Textures
    int tex_width, tex_height;
    GLuint texture1, texture2;

    glGenTextures(1, &texture1);
    unsigned char* box_image = SOIL_load_image("Textures/container.jpg", &tex_width, &tex_height, 0, SOIL_LOAD_RGB);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, box_image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(box_image);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &texture2);
    unsigned char* face_image = SOIL_load_image("Textures/awesomeface.png", &tex_width, &tex_height, 0, SOIL_LOAD_RGB);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // Set our texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, face_image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(face_image);
    glBindTexture(GL_TEXTURE_2D, 0);

    //////////////////////////////////Pre-loop configs
    Camera mainCamera(glm::vec3(0.0f, 0.0f, 3.0f));
    Shader ourShader("Shaders/shader.vert", "Shaders/shader.frag");
    ourShader.Use(); //isn't obligatory to call in game loop
    
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)screenWidth / screenHeight, 0.1f, 100.0f);
    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrameTime = glfwGetTime();
        frameDeltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        double mouseNewX, mouseNewY;
        glfwGetCursorPos(window, &mouseNewX, &mouseNewY);
        mainCamera.ProcessMouseMovement(mouseNewX - mousePrevX, -(mouseNewY - mousePrevY));
        mousePrevX = mouseNewX;
        mousePrevY = mouseNewY;

        glfwPollEvents();
        processCameraMovement(mainCamera);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture1"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture2"), 1);
      
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), (GLfloat)glfwGetTime() * 1.f, glm::vec3(0.f, 0.f, 1.f));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "rotation"), 1, GL_FALSE, glm::value_ptr(rotation));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(mainCamera.GetViewMatrix()));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
        for (GLuint i = 0; i < 10; i++)
        {
            glm::mat4 model(1.f);
            model = glm::translate(model, cubePositions[i]);
            GLfloat angle = glm::radians(20.0f) * i;
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}