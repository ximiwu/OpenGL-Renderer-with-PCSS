#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonValues.h"

#include "Window.h"
#include "Mesh.h"

#include "Shader.h"
#include "DepthShader.h"
#include "MainShader.h"
#include "GaussianBlurShader.h"
#include "FinalCompositeShader.h"

#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "Model.h"
#include "Object.h"


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


#include <assimp/Importer.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

const float toRadians = 3.14159265f / 180.0f;

const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

Window mainWindow;
std::vector<Mesh*> meshList;
Camera camera;

Texture brickTexture;
Texture dirtTexture;


Material shinyMaterial;
Material dullMaterial;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;


GLuint depthMapFBO;
GLuint depthMap;

GLuint colorAndShadowFBO;
GLuint colorTexture;        // Pass 1的颜色输出
GLuint shadowFactorTexture; // Pass 1的阴影因子输出
GLuint worldPosTexture;  // Pass1 世界空间位置输出

GLuint depthRBO;        // Pass1 深度渲染缓冲对象


GLuint blurFBO;
GLuint blurTexture;        // Pass 2的模糊阴影输出




// 用于渲染全屏四边形的VAO
GLuint quadVAO, quadVBO;

struct DebugParams {
    bool blurEnabled = true;
    float blurSpatialScale = 100.0f;
} debugParams;

MainShader mainShader;
DepthShader depthShader;
GaussianBlurShader blurShader;
FinalCompositeShader compositeShader;

void CreateShaders() {
    mainShader.CreateFromFiles("Shaders/mainShader.vert", "Shaders/mainShader.frag");
    depthShader.CreateFromFiles("Shaders/depthShader.vert", "Shaders/depthShader.frag");
    blurShader.CreateFromFiles("Shaders/screenQuadShader.vert", "Shaders/gaussianBlurShader.frag");
    compositeShader.CreateFromFiles("Shaders/screenQuadShader.vert", "Shaders/finalCompositeShader.frag");
}

void CreateScreenQuad()
{
    float quadVertices[] = {
    //view space position        //uv
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f
    };
    
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void CreateFramebuffers()
{
    // Pass 1 FBO (颜色和阴影因子)
    glGenFramebuffers(1, &colorAndShadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, colorAndShadowFBO);

    // 颜色纹理
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mainWindow.GetBufferWidth(), mainWindow.GetBufferHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

    // 阴影因子纹理
    glGenTextures(1, &shadowFactorTexture);
    glBindTexture(GL_TEXTURE_2D, shadowFactorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, mainWindow.GetBufferWidth(), mainWindow.GetBufferHeight(), 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, shadowFactorTexture, 0);

    // 创建世界位置纹理
    glGenTextures(1, &worldPosTexture);
    glBindTexture(GL_TEXTURE_2D, worldPosTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mainWindow.GetBufferWidth(), mainWindow.GetBufferHeight(), 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, worldPosTexture, 0);

    // 创建深度渲染缓冲对象
    glGenRenderbuffers(1, &depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mainWindow.GetBufferWidth(), mainWindow.GetBufferHeight());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);

    // 更新 attachments 数组以包含所有颜色附件
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // 检查 FBO 是否完整
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer is not complete!");
    }

    // Pass 2 FBO (模糊) - 不需要深度缓冲
    glGenFramebuffers(1, &blurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);

    glGenTextures(1, &blurTexture);
    glBindTexture(GL_TEXTURE_2D, blurTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, mainWindow.GetBufferWidth(), mainWindow.GetBufferHeight(), 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture, 0);

    GLuint blurAttachments[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, blurAttachments);

    // 配置深度图FBO
    glGenFramebuffers(1, &depthMapFBO);

    // 创建深度纹理
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // 将深度纹理附加到帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CheckFramebufferStatus(GLuint fbo, const char* name) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer " << name << " is not complete!" << std::endl;
    }
}

int main()
{
    Assimp::Importer importer;

    mainWindow = Window(1536, 800);
    mainWindow.Initialise();

    // 初始化 ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(mainWindow.GetWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    CreateShaders();
    CreateScreenQuad();
    CreateFramebuffers();


    CheckFramebufferStatus(colorAndShadowFBO, "colorAndShadowFBO");
    CheckFramebufferStatus(blurFBO, "blurFBO");

    camera = Camera(glm::vec3(0.0f, 0.5f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 10.0f, 0.1f);

    brickTexture = Texture("Textures/brick.png");
    brickTexture.LoadTextureA();
    dirtTexture = Texture("Textures/dirt.png");
    dirtTexture.LoadTextureA();


    shinyMaterial = Material(1.0f, 32);
    dullMaterial = Material(0.5f, 4);

    Model model_marry;
    model_marry.LoadModel("Models/Marry.obj");
    Model model_floor;
    model_floor.LoadModel("Models/floor.obj");
    Model model_cube;
    model_cube.LoadModel("Models/cube.obj");


    Object marry(dullMaterial, model_marry);
    marry.set_model_matrix(glm::vec3(0.0f, 0.1f, 0.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    Object floor(dullMaterial, model_floor);
    floor.set_model_matrix(glm::vec3(0.0f, 0.1f, 0.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    Object cube(dullMaterial, model_cube);


    mainLight = DirectionalLight(
        1.0f, 1.0f, 1.0f, 
        0.0f, 0.0f, 
        0.0f, -0.0f, -1.0f
    );


    unsigned int pointLightCount = 0;

    pointLights[0] = PointLight(
        1.0f, 1.0f, 1.0f,
        1.5f, 0.5f,
        2.0f, 6.0f, 2.0f,
        1.0f, 0.2f, 0.05f,
        10.0f,
        SHADOW_WIDTH, SHADOW_HEIGHT
    );
    pointLightCount += 1;

    unsigned int spotLightCount = 0;



    glm::mat4 projection = glm::perspective(45.0f, mainWindow.GetBufferWidth() / mainWindow.GetBufferHeight(), 0.1f, 100.0f);

    while (!mainWindow.GetShouldClose()) {
        //delta time
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;

        //player control
        glfwPollEvents();

        if (camera.light_moving) {
            //npc control
            float time = glfwGetTime();

            // Calculate yaw and pitch angles that oscillate between -30 and 30 degrees
            float yaw = 30.0f * sin(time); // Oscillates between -30 and 30
            float pitch = 45.0f * sin(time * 0.5f); // Oscillates between -30 and 30, slower than yaw

            // Convert angles to radians
            float yawRad = yaw * toRadians;
            float pitchRad = pitch * toRadians;

            // Calculate the position on the sphere
            float radius = 3.0f; // Distance from center (0,0,0)
            float x = radius * cos(pitchRad) * sin(yawRad);
            float y = radius * sin(pitchRad);
            float z = radius * cos(pitchRad) * cos(yawRad);

            // Update point light position
            pointLights[0].SetPosition(x, y + 6.0f, z); // Added y offset to raise light above ground
        }
        camera.KeyControl(mainWindow.GetKeys(), deltaTime);
        if (camera.cursor_disable) {
            glfwSetInputMode(mainWindow.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            camera.MouseControl(mainWindow.GetXChange(), mainWindow.GetYChange());
        }
        else {
            glfwSetInputMode(mainWindow.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mainWindow.IgnoreXYChange();
        }

        GLboolean blurOn = debugParams.blurEnabled;
        GLfloat blurSpatialScale = debugParams.blurSpatialScale;
        


        glEnable(GL_DEPTH_TEST);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        //render depth map
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        // 使用深度着色器
        depthShader.UseShader();

        depthShader.SetShaderEnvironmentUniform(pointLights[0].GetLightSpaceMatrix());

        // 渲染场景
        depthShader.SetShaderModelUniform(marry.get_model_matrix());
        marry.render_this();
        depthShader.SetShaderModelUniform(floor.get_model_matrix());
        floor.render_this();

        // Pass 1: 渲染场景到colorAndShadowFBO
        glBindFramebuffer(GL_FRAMEBUFFER, colorAndShadowFBO);
        GLuint mainShaderAttachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
        glDrawBuffers(3, mainShaderAttachments);

        glViewport(0, 0, mainWindow.GetBufferWidth(), mainWindow.GetBufferHeight());

        // 清除颜色纹理为黑色
        glClearBufferfv(GL_COLOR, 0, glm::value_ptr(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
        // 清除阴影因子纹理为1.0
        float shadowClearValue = 1.0f;
        glClearBufferfv(GL_COLOR, 1, &shadowClearValue);
        // 清除世界位置纹理为一个在half float范围内的"无限远"点
        glm::vec3 infinitePos(65504.0f, 65504.0f, 65504.0f);  // 半精度浮点数的最大值
        glClearBufferfv(GL_COLOR, 2, glm::value_ptr(infinitePos));
        // 清除深度缓冲
        glClear(GL_DEPTH_BUFFER_BIT);

        mainShader.UseShader();
        mainShader.SetShaderEnvironmentUniform(&mainLight, 
            pointLights, pointLightCount, 
            spotLights, spotLightCount, 
            camera.GetCameraLocation(), projection, camera.CalculateViewMatrix(), 
            pointLights[0].GetLightSpaceMatrix(), depthMap, pointLights[0].GetSphericalLightRadius());

        // 渲染场景...
        mainShader.SetShaderModelUniform(marry.get_model_matrix(), marry.get_material());
        marry.render_this();
        mainShader.SetShaderModelUniform(floor.get_model_matrix(), floor.get_material());
        floor.render_this();
        cube.set_model_matrix(pointLights[0].GetPosition(), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f));
        mainShader.SetShaderModelUniform(cube.get_model_matrix(), cube.get_material());
        cube.render_this();

        // 禁用深度测试（因为是屏幕空间渲染）
        glDisable(GL_DEPTH_TEST);

        // Pass 2: 模糊
        blurShader.UseShader();
        glBindVertexArray(quadVAO);

        // 水平模糊
        glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);
        glClearBufferfv(GL_COLOR, 0, &shadowClearValue);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shadowFactorTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, worldPosTexture);
        blurShader.SetShaderEnvironmentUniform(GL_TRUE, 0, 1, blurOn, blurSpatialScale);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // 垂直模糊
        glBindFramebuffer(GL_FRAMEBUFFER, colorAndShadowFBO);
        // 清除阴影因子纹理为1.0
        glClearBufferfv(GL_COLOR, 1, &shadowClearValue);
        GLuint blurShaderAttachments[] = { GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(1, blurShaderAttachments);

        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, blurTexture);
        blurShader.SetShaderEnvironmentUniform(GL_FALSE, 0, 1, blurOn, blurSpatialScale);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Pass 3: 最终合成
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        compositeShader.UseShader();
        
        // 设置颜色纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorTexture);

        // 设置阴影因子纹理
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowFactorTexture);

        compositeShader.SetShaderEnvironmentUniform(1, 0);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        

        glBindVertexArray(0);
        glUseProgram(0);
        
        // ImGui 新帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 创建调试窗口
        ImGui::Begin("Parameters");
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Checkbox("Enable Blur", &debugParams.blurEnabled);
        ImGui::InputFloat("Blur Spatial Scale", &debugParams.blurSpatialScale, 0.1f, 1.0f, "%.3f");
        ImGui::End();

        // 渲染 ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        mainWindow.SwapBuffers();
        
        
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}

