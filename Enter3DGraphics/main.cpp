#include "Shader.h"

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <soil.h>
#include <iostream>

// GLEW VERSION IS 4.0

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // ����� ������������ �������� ESC, �� ������������� �������� WindowShouldClose � true, 
    // � ���������� ����� ����� ���������
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main()
{
    //������������� GLFW
    glfwInit();
    //��������� GLFW
    //�������� ����������� ��������� ������ OpenGL. 
    //�������� 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //��������
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //��������� �������� ��� �������� ��������� ��������
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //���������� ����������� ��������� ������� ����
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

    Shader ourShader("Shaders/simple_shader.vert", "Shaders/colors_shader.frag");

    GLfloat vertices[] = {
        // �������          // �����             // ���������� ����������
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.f, 1.0f,   // ������� ������
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.f, 0.0f,   // ������ ������
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // ������ �����
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // ������� �����
    };
    GLuint indices[] = {  // Note that we start from 0!
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };

    // Load, create texture and generate mipmaps
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


    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // ������� � ������������
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture1"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture2"), 1);

        glm::mat4 model(1.f);
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 view(1.f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        glm::mat4 projection(1.f);
        projection = glm::perspective(glm::radians(45.0f), (GLfloat)screenWidth / screenHeight, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 rotation(1.f);
        rotation = glm::rotate(rotation, (GLfloat)glfwGetTime() * 0.5f, glm::vec3(0.f, 0.f, 1.f));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "rotation"), 1, GL_FALSE, glm::value_ptr(rotation));
        ourShader.Use();

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}