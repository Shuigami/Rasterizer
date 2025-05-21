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

void load_scene(Shader& shader) {
    Camera camera(
        Vec3(0.0f, 1.0f, 5.0f),
        Vec3(0.0f, 1.0f, 0.0f),
        Vec3(0.0f, 1.0f, 0.0f),
        60.0f * (3.14159f / 180.0f),
        static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT,
        0.1f,
        100.0f
    );

    shader.clearLights();

    LOG_INFO("Configuring lighting...");
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

    shader.addLight(pointLight);
    // shader.addLight(pointLight2);

    LOG_INFO("Lighting configured successfully");

    Vec3 cameraPos = camera.getPosition();
    shader.setCameraPosition(cameraPos);
    shader.setViewMatrix(camera.getViewMatrix());
    shader.setProjectionMatrix(camera.getProjectionMatrix());
}

void load_shaders(Rasterizer& rasterizer) {
    LOG_INFO("Setting up shaders...");
    PhongShader* phongShader = new PhongShader();
    phongShader->setAmbient(0.2f);
    phongShader->setDiffuse(0.7f);
    phongShader->setSpecular(0.5f);
    phongShader->setShininess(32.0f);

    ToonShader* toonShader = new ToonShader();
    toonShader->setLevels(2);
    toonShader->setOutlineThickness(0.2f);
    toonShader->setOutlineColor(Color(0, 0, 0, 255));
    toonShader->setEnableOutline(true);
    toonShader->setAmbient(0.3f);
    toonShader->setDiffuse(0.8f);
    toonShader->setSpecular(0.5f);

    FlatShader* flatShader = new FlatShader(Color(200, 50, 50));

    rasterizer.addShader(phongShader);
    rasterizer.addShader(toonShader);
    rasterizer.addShader(flatShader);

    load_scene(*phongShader);
    load_scene(*toonShader);
    load_scene(*flatShader);

    rasterizer.setCurrentShader(0);
    LOG_INFO("Shaders loaded successfully");
}

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
    LOG_INFO("Rasterizer initialized successfully");
    load_shaders(rasterizer);

    LOG_INFO("Loading meshes...");
    Mesh cubeMesh;
    cubeMesh.createCube(Color(80, 80, 80));

    Mesh sphereMesh;
    sphereMesh.createSphere(16, 16, Color(50, 50, 200));

    Mesh planeMesh;
    planeMesh.createPlane(5.0f, 5.0f, Color(255, 0, 0));

    Mesh triangleMesh;
    triangleMesh.createTriangle(5.5f, 5.5f, Color(0, 0, 255));

    Mesh swordMesh;
    swordMesh.loadFromOBJ("assets/sword.obj");

    cubeMesh.setModelMatrix(Matrix4x4::translation(0.0f, -1.0f, 0.0f));
    sphereMesh.setModelMatrix(Matrix4x4::scaling(5.0f, 5.0f, 5.0f));
    planeMesh.setModelMatrix(Matrix4x4::translation(0.0f, -0.5f, 0.0f));
    swordMesh.setModelMatrix(Matrix4x4::scaling(0.05f, 0.05f, 0.05f) * Matrix4x4::rotationX(M_PI / 2.0f) * Matrix4x4::translation(0.0f, -4.0f, 0.0f));
    LOG_INFO("All meshes loaded successfully");

    float rotation = 0.0f;
    uint32_t lastTick = SDL_GetTicks();
    LOG_INFO("Starting render loop");
    while (!rasterizer.shouldQuit()) {
        rasterizer.handleEvents();

        uint32_t currentTick = SDL_GetTicks();
        float deltaTime = (currentTick - lastTick) / 1000.0f;
        lastTick = currentTick;
        rotation += 0.7f * deltaTime;

        sphereMesh.setModelMatrix(Matrix4x4::rotationY(rotation) * Matrix4x4::translation(1.0f, 0.0f, 0.0f));

        rasterizer.clear(Color(20, 20, 20));

        rasterizer.beginShadowPass();

        rasterizer.renderShadowMap(sphereMesh, *rasterizer.getCurrentShader());
        uint32_t current = SDL_GetTicks();
        rasterizer.renderShadowMap(planeMesh, *rasterizer.getCurrentShader());
        LOG_INFO("Done rendering shadow map for plane in " + std::to_string(SDL_GetTicks() - current) + " ms");
        // rasterizer.renderShadowMap(swordMesh, *rasterizer.getCurrentShader());

        rasterizer.renderMesh(sphereMesh, *rasterizer.getCurrentShader());
        current = SDL_GetTicks();
        rasterizer.renderMesh(planeMesh, *rasterizer.getCurrentShader());
        LOG_INFO("Done rendering plane in " + std::to_string(SDL_GetTicks() - current) + " ms");
        // rasterizer.renderMesh(swordMesh, *currentShader);

        rasterizer.present();
    }

    LOG_INFO("Shutting down application");
    SDL_Quit();
    return 0;
}
