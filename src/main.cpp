#include <SDL.h>
#include "rasterizer.h"
#include "mesh.h"
#include "shader.h"
#include "camera.h"
#include "vector.h"
#include "matrix.h"
#include "logger.h"

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
    planeMesh.createPlane(1.0f, 1.0f, Color(255, 255, 255));
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

    Shader* currentShader = &phongShader;

    Light pointLight;
    pointLight.type = Light::Type::Point;
    pointLight.position = Vec3(5.0f, 2.0f, 5.0f);
    pointLight.color = Color(255, 255, 255);
    pointLight.intensity = 1.f;
    pointLight.range = 20.0f;

    currentShader->addLight(pointLight);
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

        Matrix4x4 cubeModelMatrix = Matrix4x4::translation(0.0f, 0.5f, 0.0f);
        Matrix4x4 sphereModelMatrix = Matrix4x4::translation(0.0f, 0.0f, 0.0f);
        Matrix4x4 wellModelMatrix = Matrix4x4::translation(0.0f, -0.5f, 0.0f) * Matrix4x4::rotationY(M_PI / 4) * Matrix4x4::scaling(0.3f, 0.3f, 0.3f);
        Matrix4x4 planeModelMatrix = Matrix4x4::translation(0.0f, -1.0f, 0.0f) * Matrix4x4::scaling(20.0f, 1.0f, 20.0f);
        Matrix4x4 planeBgModelMatrix = Matrix4x4::rotationX(M_PI / 2) * Matrix4x4::scaling(1.0f, 1.0f, 5.8f);

        rasterizer.clear(Color(20, 20, 20));

        // currentShader->setModelMatrix(cubeModelMatrix);
        // rasterizer.renderMesh(cubeMesh, *currentShader);

        // currentShader->setModelMatrix(sphereModelMatrix);
        // rasterizer.renderMesh(sphereMesh, *currentShader);

        // currentShader->setModelMatrix(wellModelMatrix);
        // rasterizer.renderMesh(wellMesh, *currentShader);
        
        // currentShader->setModelMatrix(planeModelMatrix);
        // rasterizer.renderMesh(planeMesh, *currentShader);

        currentShader->setModelMatrix(planeBgModelMatrix);
        rasterizer.renderMesh(planeMesh, *currentShader);

        rasterizer.present();

        SDL_Delay(16);
    }

    LOG_INFO("Shutting down application");
    SDL_Quit();
    return 0;
}
