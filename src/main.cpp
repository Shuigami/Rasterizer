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
        Vec3(0.0f, 0.0f, 5.0f),
        Vec3(0.0f, 0.0f, 0.0f),
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

    Mesh wellMesh;
    wellMesh.loadFromOBJ("assets/moto.obj");

    Mesh planeMesh;
    planeMesh.createPlane(10.0f, 10.0f, Color(255, 0, 0));

    Mesh triangleMesh;
    triangleMesh.createTriangle(1.0f, 1.0f, Color(0, 0, 255));

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
    pointLight.position = Vec3(1.0f, 2.0f, 0.0f);
    pointLight.color = Color(255, 255, 255);
    pointLight.intensity = 1.2f;
    pointLight.range = 20.0f;

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
    LOG_INFO("Shaders and lighting configured");

    float rotation = 0.0f;
    uint32_t lastTick = SDL_GetTicks();
    LOG_INFO("Starting render loop");

    while (!rasterizer.shouldQuit()) {
        rasterizer.handleEvents();

        Vec3 cameraPos = camera.getPosition();

        uint32_t currentTick = SDL_GetTicks();
        float deltaTime = (currentTick - lastTick) / 1000.0f;
        lastTick = currentTick;

        rotation += 0.7f * deltaTime;
        
        currentShader->setCameraPosition(cameraPos);
        currentShader->setViewMatrix(camera.getViewMatrix());
        currentShader->setProjectionMatrix(camera.getProjectionMatrix());

        pointLight.position = Vec3(
            5.0f * std::cos(rotation),
            2.0f,
            5.0f * std::sin(rotation)
        );

        currentShader->clearLights();
        currentShader->addLight(pointLight);

        Matrix4x4 cubeModelMatrix = Matrix4x4::translation(0.0f, -1.0f, 0.0f);
        Matrix4x4 sphereModelMatrix = Matrix4x4::translation(0.0f, 0.0f, 0.0f);
        Matrix4x4 planeModelMatrix = Matrix4x4::translation(0.0f, -0.5f, 0.0f);
        Matrix4x4 planeModelMatrix2 = Matrix4x4::identity();

        rasterizer.clear(Color(20, 20, 20));
        
        Vec3 lightPos = currentLight->position;
        Vec3 lightDir = Vec3(0.0f, 0.0f, 0.0f);

        if (currentLight->type == Light::Type::Point) {
            lightDir = (Vec3(0.0f, 0.0f, 0.0f) - lightPos).normalized();
        } else if (currentLight->type == Light::Type::Directional || currentLight->type == Light::Type::Spot) {
            lightDir = currentLight->direction.normalized();
        }
        
        rasterizer.beginShadowPass();
        
        currentShader->setModelMatrix(sphereModelMatrix);
        rasterizer.renderShadowMap(sphereMesh, *currentShader, lightPos, lightDir);
        
        currentShader->setModelMatrix(planeModelMatrix);
        rasterizer.renderShadowMap(planeMesh, *currentShader, lightPos, lightDir);

        currentShader->setModelMatrix(sphereModelMatrix);
        rasterizer.renderMesh(sphereMesh, *currentShader);

        currentShader->setModelMatrix(planeModelMatrix);
        rasterizer.renderMesh(planeMesh, *currentShader);

        rasterizer.present();
    }

    LOG_INFO("Shutting down application");
    SDL_Quit();
    return 0;
}
