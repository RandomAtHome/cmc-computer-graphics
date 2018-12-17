#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "MeshGenerators.h"
#include "cube_vertices.h"

#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// GLEW VERSION IS 4.0

bool pressedKeys[1024];
GLfloat lastTimePressed[1024] = {0};
GLfloat frameDeltaTime = 0.0f;
GLfloat lastFrameTime = 0.0; 
const GLfloat KEY_PRESS_THRESHOLD = 0.2;

bool isFlashlightOn = false;
bool isFigureReflecting = true;
bool isParallaxSelfShadowing = false;
double mousePrevX, mousePrevY;
Camera mainCamera(glm::vec3(0.0f, 0.0f, 3.0f));
const unsigned int screenWidth = 1280;
const unsigned int screenHeight = 720;
int DebugLevel = 0;

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
    if (DebugLevel > 1) {
        std::cout << "Camera direction:" << std::endl <<
        mainCamera.Front.x << " " << mainCamera.Front.y << " " << mainCamera.Front.z << std::endl;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
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
        if (lastFrameTime - lastTimePressed[GLFW_KEY_F] > KEY_PRESS_THRESHOLD) {
            isFlashlightOn ^= 1;
            lastTimePressed[GLFW_KEY_F] = lastFrameTime;
        }
    }
    if (pressedKeys[GLFW_KEY_P]) {
        if (lastFrameTime - lastTimePressed[GLFW_KEY_P] > KEY_PRESS_THRESHOLD) {
            isFigureReflecting ^= 1;
            lastTimePressed[GLFW_KEY_P] = lastFrameTime;
        }
    }
    if (pressedKeys[GLFW_KEY_T]) {
        if (lastFrameTime - lastTimePressed[GLFW_KEY_T] > KEY_PRESS_THRESHOLD) {
            isParallaxSelfShadowing ^= 1;
            lastTimePressed[GLFW_KEY_T] = lastFrameTime;
        }
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Computer Graphics, Chukharev 301", nullptr, nullptr);
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

    //////////////////////////////////Skybox VAO and VBO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    vector<std::string> faces{
        "posx.tga",
        "negx.tga",
        "posy.png",
        "negy.png",
        "posz.tga",
        "negz.tga"
    };
    unsigned int cubemapTexture = loadCubemap(faces, "Textures/Skybox");

    //////////////////////////////////Regular stuff creation
    Shader skyboxShader("Shaders/Skybox/skybox.vert", "Shaders/Skybox/skybox.frag");
    Shader parallaxShader("Shaders/ParallaxMapping/pm_quad.vert", "Shaders/ParallaxMapping/pm_quad.frag");
    Shader normalShader("Shaders/NormalMapping/nm_quad.vert", "Shaders/NormalMapping/nm_quad.frag");
    Shader modelShader("Shaders/SkyboxReflection/shader.vert", "Shaders/SkyboxReflection/shader.frag");
    Shader cubeLampShader("Shaders/simpleShader.vert", "Shaders/light_cube.frag");
    Model ourModel("Objects/Bench/bench.obj");
    
    vector<Texture> textures;
    Texture texture;
    texture.type = "texture_diffuse";
    texture.path = "Textures/Bricks/bricks.jpg";
    texture.id = TextureFromFile("bricks.jpg", "Textures/Bricks");
    textures.push_back(texture);
    texture.type = "texture_normal";
    texture.path = "Textures/Bricks/bricks_NORMAL.jpg";
    texture.id = TextureFromFile("bricks_NORMAL.jpg", "Textures/Bricks");
    textures.push_back(texture);
    texture.type = "texture_height";
    texture.path = "Textures/Bricks/bricks_DISP.jpg";
    texture.id = TextureFromFile("bricks_DISP.jpg", "Textures/Bricks");
    textures.push_back(texture);
    Mesh parallaxBrickWall = createQuadMesh(textures);
    textures.clear();

    texture.type = "texture_diffuse";
    texture.path = "Textures/Blackwood/blackwood.jpg";
    texture.id = TextureFromFile("blackwood.jpg", "Textures/Blackwood");
    textures.push_back(texture);
    texture.type = "texture_normal";
    texture.path = "Textures/Blackwood/blackwood_NORMAL.jpg";
    texture.id = TextureFromFile("blackwood_NORMAL.jpg", "Textures/Blackwood");
    textures.push_back(texture);
    texture.type = "texture_specular";
    texture.path = "Textures/Blackwood/blackwood_SPECULAR.jpg";
    texture.id = TextureFromFile("blackwood_SPECULAR.jpg", "Textures/Blackwood");
    textures.push_back(texture);
    Mesh normalWoodenFloor = createQuadMesh(textures);
    Mesh normalWoodenBenchPost = createQuadMesh(textures);
    textures.clear();
    Mesh flyingCubeLamp = createCubeMesh(textures);
    
    //////////////////////////////////Pre-loop configs
    skyboxShader.Use();
    skyboxShader.setInt("skybox", 0);
    modelShader.Use();
    modelShader.setInt("skybox", 0);

    // lighting info
    glm::vec3 oldLightPos(0.5f, 1.0f, 0.3f);
    glm::vec3 lightPos(0.5f, 1.0f, 0.3f);
    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrameTime = glfwGetTime();
        frameDeltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;
        
        processCameraMovement(mainCamera);
        processActionKeys();

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model;
        glm::mat4 projection = glm::perspective(glm::radians(mainCamera.Zoom), (GLfloat)screenWidth / screenHeight, 0.1f, 100.0f);
        glm::mat4 view = mainCamera.GetViewMatrix();

        lightPos = oldLightPos;
        lightPos.x += 2 * glm::sin(lastFrameTime);
        lightPos.y += 2.5 * glm::cos(lastFrameTime);

        parallaxShader.Use();
        parallaxShader.setMat4("projection", projection);
        parallaxShader.setMat4("view", view);
  
        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(0.f, 0.f, -2.f));
        model = glm::scale(model, glm::vec3(2.f));
        parallaxShader.setMat4("model", model);
        parallaxShader.setVec3("viewPos", mainCamera.Position);
        parallaxShader.setVec3("lightPos", lightPos);
        parallaxShader.setFloat("heightScale", 0.1f);
        parallaxShader.setInt("selfShadowState", isParallaxSelfShadowing);
        parallaxBrickWall.Draw(parallaxShader);

        normalShader.Use();
        normalShader.setMat4("projection", projection);
        normalShader.setMat4("view", view);
        normalShader.setVec3("viewPos", mainCamera.Position);
        normalShader.setVec3("lightPos", lightPos);
        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(0.f, -1.9f, 0.f));
        model = glm::rotate(model, (GLfloat)glm::radians(270.), glm::vec3(1.f, 0.f, 0.f));
        model = glm::scale(model, glm::vec3(2.f));
        normalShader.setMat4("model", model);
        normalWoodenFloor.Draw(normalShader);

        normalShader.Use();
        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(0.f, -2.f, 6.f));
        model = glm::rotate(model, (GLfloat)glm::radians(270.), glm::vec3(1.f, 0.f, 0.f));
        model = glm::scale(model, glm::vec3(2.f));
        normalShader.setMat4("model", model);
        normalWoodenBenchPost.Draw(normalShader);

        modelShader.Use();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(0.0f, -2.f, 6.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
        modelShader.setMat4("model", model);
        modelShader.setVec3("cameraPos", mainCamera.Position);
        modelShader.setInt("reflectState", isFigureReflecting);
        ourModel.Draw(modelShader);

        cubeLampShader.Use();
        cubeLampShader.setMat4("projection", projection);
        cubeLampShader.setMat4("view", view);
        model = glm::mat4(1.f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.05f));
        parallaxShader.setMat4("model", model);
        flyingCubeLamp.Draw(cubeLampShader);

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.Use();
        view = glm::mat4(glm::mat3(mainCamera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
