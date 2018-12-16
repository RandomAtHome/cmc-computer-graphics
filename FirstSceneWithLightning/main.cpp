#include "Shader.h"
#include "Camera.h"
#include "cube_vertices.h"

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

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = SOIL_load_image(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        SOIL_free_image_data(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        SOIL_free_image_data(data);
    }

    return textureID;
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
    GLuint VBO, VAO, lightVAO;

    glGenBuffers(1, &VBO);
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    //////////////////////////////////Pre-loop configs
    Camera mainCamera(glm::vec3(0.0f, 0.0f, 3.0f));
    Shader ourShader("Shaders/shader.vert", "Shaders/shader.frag");
    Shader lightingShader("Shaders/shader.vert", "Shaders/light_cube.frag");
   
    unsigned int diffuseMap = loadTexture("Textures/container2.png");
    unsigned int specularMap = loadTexture("Textures/container2_specular.png");

    ourShader.Use();
    ourShader.setInt("m_diffuse", 0);
    ourShader.setInt("m_specular", 1);

    glm::vec3 lighting_position(1.f, 1.5f, -1.f);
    glm::vec3 box_color(1.0f, 0.5f, 0.31f);
     //isn't obligatory to call in game loop

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrameTime = glfwGetTime();
        frameDeltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        glfwPollEvents();
        processCameraMovement(mainCamera);

        double mouseNewX, mouseNewY;
        glfwGetCursorPos(window, &mouseNewX, &mouseNewY);
        mainCamera.ProcessMouseMovement(mouseNewX - mousePrevX, -(mouseNewY - mousePrevY));
        mousePrevX = mouseNewX;
        mousePrevY = mouseNewY;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(mainCamera.Zoom), (GLfloat)screenWidth / screenHeight, 0.1f, 100.0f);

        glBindVertexArray(lightVAO);
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), (GLfloat)glm::radians(0.05f), glm::vec3(0.f, 0.f, 1.f));
        rotation = glm::rotate(rotation, (GLfloat)glm::radians(0.05f*glm::sin(glfwGetTime())), glm::vec3(1.f, 0.f, 0.f));
        lighting_position = rotation * glm::vec4(lighting_position, 1.f);
        lightingShader.Use();
        lightingShader.setMat4("view", mainCamera.GetViewMatrix());
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("model", glm::translate(glm::mat4(1.f), lighting_position) * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);



        glBindVertexArray(VAO);
        ourShader.Use();
        ourShader.setFloat("m_shininess", 64.f);

        // light properties
        ourShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        ourShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("light.position", lighting_position);
        ourShader.setVec3("viewPos", mainCamera.Position);
        
        ourShader.setMat4("view", mainCamera.GetViewMatrix());
        ourShader.setMat4("projection", projection);
        
        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        for (GLuint i = 0; i < 1; i++)
        { 
            ourShader.setMat4("model", glm::rotate(
                glm::translate(glm::mat4(1.f), cubePositions[i]), glm::radians(20.0f) * i, glm::vec3(1.0f, 0.3f, 0.5f)
            ));
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