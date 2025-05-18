#include <SDL.h>
#include "rasterizer.h"
#include "mesh.h"
#include "shader.h"
#include "camera.h"
#include "vector.h"
#include "matrix.h"
#include <logger.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
bool wireframeMode = false;

int main(int argc, char** argv) {
    Logger& logger = Logger::getInstance();
    logger.setLevel(LogLevel::INFO);
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        LOG_ERROR("SDL initialization failed: " + std::string(SDL_GetError()));
        return 1;
    }

    LOG_INFO("Starting rasterizer...");
    Rasterizer rasterizer(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!rasterizer.initialize()) {
        LOG_ERROR("Failed to initialize rasterizer.");
        SDL_Quit();
        return 1;
    }

    Camera camera(
        Vec3(0.0f, 1.0f, 5.0f),
        Vec3(0.0f, 1.0f, 0.0f),
        Vec3(0.0f, 1.0f, 0.0f),
        60.0f * (3.14159f / 180.0f),
        static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT,
        0.1f,
        100.0f
    );
    LOG_INFO("Camera initialized");

    LOG_INFO("Loading meshes...");
    Mesh cubeMesh;
    cubeMesh.createCube(Color(80, 80, 80));

    Mesh sphereMesh;
    sphereMesh.createSphere(16, 16, Color(50, 50, 200));

    Mesh planeMesh;
    planeMesh.createPlane(2.0f, 2.0f, Color(255, 0, 0));

    Mesh triangleMesh;
    triangleMesh.createTriangle(5.5f, 5.5f, Color(0, 0, 255));

    LOG_INFO("All meshes loaded successfully");

    LOG_INFO("Setting up shaders...");
    PhongShader phongShader;
    phongShader.setAmbient(0.2f);
    phongShader.setDiffuse(0.7f);
    phongShader.setSpecular(0.5f);
    phongShader.setShininess(32.0f);

    ToonShader toonShader;
    toonShader.setLevels(4);
    toonShader.setOutlineThickness(0.2f);
    toonShader.setOutlineColor(Color(0, 0, 0, 255));
    toonShader.setEnableOutline(true);
    toonShader.setAmbient(0.3f);
    toonShader.setDiffuse(0.8f);
    toonShader.setSpecular(0.5f);

    FlatShader flatShader(Color(200, 50, 50));

    Shader* currentShader = &toonShader;

    Light pointLight;
    pointLight.type = Light::Type::Point;
    pointLight.position = Vec3(2.0f, 2.0f, 2.0f);
    pointLight.color = Color(255, 255, 255);
    pointLight.intensity = 1.2f;
    pointLight.range = 20.0f;

    Light pointLight2;
    pointLight2.type = Light::Type::Point;
    pointLight2.position = Vec3(-2.0f, 2.0f, 2.0f);
    pointLight2.color = Color(255, 255, 255);
    pointLight2.intensity = 1.2f;
    pointLight2.range = 20.0f;

    Light spotLight;
    spotLight.type = Light::Type::Spot;
    spotLight.position = Vec3(0.0f, 2.0f, 0.0f);
    spotLight.direction = Vec3(0.0f, -1.0f, 0.0f);
    spotLight.color = Color(255, 255, 255);
    spotLight.intensity = 1.2f;
    spotLight.range = 20.0f;
    spotLight.spotAngle = 0.5f;

    Light *currentLight = &pointLight;

    currentShader->addLight(*currentLight);
    currentShader->addLight(pointLight2);

    LOG_INFO("Shaders and lighting configured");

    float rotation = 0.0f;
    uint32_t lastTick = SDL_GetTicks();
    LOG_INFO("Starting render loop");

    cubeMesh.setModelMatrix(Matrix4x4::translation(0.0f, -1.0f, 0.0f));
    // sphereMesh.setModelMatrix(Matrix4x4::translation(0.0f, -1.0f, 0.0f));
    planeMesh.setModelMatrix(Matrix4x4::translation(0.0f, -0.5f, 0.0f));

    Vec3 cameraPos = camera.getPosition();
    currentShader->setCameraPosition(cameraPos);
    currentShader->setViewMatrix(camera.getViewMatrix());
    currentShader->setProjectionMatrix(camera.getProjectionMatrix());

    while (!rasterizer.shouldQuit()) {
        rasterizer.handleEvents();

        // uint32_t currentTick = SDL_GetTicks();
        // float deltaTime = (currentTick - lastTick) / 1000.0f;
        // lastTick = currentTick;

        // rotation += 0.7f * deltaTime;

        // currentLight->position = Vec3(
        //     5.0f * std::cos(rotation),
        //     2.0f,
        //     5.0f 
        // );

        // currentShader->clearLights();
        // currentShader->addLight(*currentLight);

        rasterizer.clear(Color(20, 20, 20));

        // sphereMesh.setModelMatrix(Matrix4x4::translation(1.0f, std::sin(rotation), 0.0f));
        
        rasterizer.beginShadowPass();
        
        rasterizer.renderShadowMap(sphereMesh, *currentShader);
        rasterizer.renderShadowMap(planeMesh, *currentShader);

        rasterizer.renderMesh(sphereMesh, *currentShader);
        rasterizer.renderMesh(planeMesh, *currentShader);

        rasterizer.present();
    }

    LOG_INFO("Shutting down application");
    SDL_Quit();
    return 0;
}
