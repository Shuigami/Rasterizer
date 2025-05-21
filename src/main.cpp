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

    shader.addLight(pointLight);

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

    FlatShader* flatShader = new FlatShader();

    rasterizer.addShader(phongShader);
    rasterizer.addShader(toonShader);
    rasterizer.addShader(flatShader);

    load_scene(*phongShader);
    load_scene(*toonShader);
    load_scene(*flatShader);

    rasterizer.setCurrentShader(0);
    rasterizer.setShadowsEnabled(false);
    LOG_INFO("Shaders loaded successfully");
}

void scene_1(Rasterizer& rasterizer) {
    Mesh sphereMesh;
    sphereMesh.createSphere(16, 16, Color(50, 50, 200));

    Light pointLight;
    pointLight.type = Light::Type::Point;
    pointLight.position = Vec3(2.0f, 2.0f, 2.0f);
    pointLight.color = Color(255, 255, 255);
    pointLight.intensity = 1.2f;
    pointLight.range = 20.0f;

    float rotation = 0.0f;
    uint32_t lastTick = SDL_GetTicks();
    while (!rasterizer.shouldQuit()) {
        rasterizer.handleEvents();

        uint32_t currentTick = SDL_GetTicks();
        float deltaTime = (currentTick - lastTick) / 1000.0f;
        lastTick = currentTick;
        rotation += 0.7f * deltaTime;

        pointLight.position = Vec3(2.0f * cos(rotation), 2.0f, 2.0f * sin(rotation));

        rasterizer.getCurrentShader()->clearLights();
        rasterizer.getCurrentShader()->addLight(pointLight);

        rasterizer.clear(Color(20, 20, 20));

        rasterizer.beginShadowPass();
        rasterizer.renderShadowMap(sphereMesh, *rasterizer.getCurrentShader());
        rasterizer.renderMesh(sphereMesh, *rasterizer.getCurrentShader());
        rasterizer.present();
    }
}

void scene_2(Rasterizer& rasterizer) {
    Mesh sphereMesh;
    sphereMesh.createSphere(16, 16, Color(50, 50, 200));

    Mesh planeMesh;
    planeMesh.createPlane(5.0f, 5.0f, Color(255, 0, 0));
    planeMesh.setModelMatrix(Matrix4x4::translation(0.0f, -0.5f, 0.0f));

    float rotation = 0.0f;
    uint32_t lastTick = SDL_GetTicks();
    while (!rasterizer.shouldQuit()) {
        rasterizer.handleEvents();

        uint32_t currentTick = SDL_GetTicks();
        float deltaTime = (currentTick - lastTick) / 1000.0f;
        lastTick = currentTick;
        rotation += 0.7f * deltaTime;

        sphereMesh.setModelMatrix(Matrix4x4::rotationY(rotation) * Matrix4x4::translation(1.0f, 0.0f, 0.0f));

        rasterizer.clear(Color(20, 20, 20));

        rasterizer.beginShadowPass();
        rasterizer.renderShadowMap(planeMesh, *rasterizer.getCurrentShader());
        rasterizer.renderShadowMap(sphereMesh, *rasterizer.getCurrentShader());
        rasterizer.renderMesh(planeMesh, *rasterizer.getCurrentShader());
        rasterizer.renderMesh(sphereMesh, *rasterizer.getCurrentShader());
        rasterizer.present();
    }
}

void scene_3(Rasterizer& rasterizer) {
    Mesh wellMesh;
    wellMesh.loadFromOBJ("assets/well.obj");
    wellMesh.setModelMatrix(Matrix4x4::scaling(0.1f, 0.1f, 0.1f));

    float rotation = 0.0f;
    uint32_t lastTick = SDL_GetTicks();
    while (!rasterizer.shouldQuit()) {
        rasterizer.handleEvents();

        uint32_t currentTick = SDL_GetTicks();
        float deltaTime = (currentTick - lastTick) / 1000.0f;
        lastTick = currentTick;
        rotation += 0.7f * deltaTime;

        wellMesh.setModelMatrix(Matrix4x4::rotationY(rotation) * Matrix4x4::translation(0.0f, -1.0f, 0.0f) * Matrix4x4::scaling(0.1f, 0.1f, 0.1f));

        rasterizer.clear(Color(20, 20, 20));

        rasterizer.renderMesh(wellMesh, *rasterizer.getCurrentShader());
        rasterizer.present();
    }
}

void scene_4(Rasterizer& rasterizer) {
    Mesh sunMesh;
    sunMesh.createSphere(16, 16, Color(255, 255, 0));

    Mesh mercuryMesh;
    mercuryMesh.createSphere(16, 16, Color(150, 150, 150));
    mercuryMesh.setModelMatrix(Matrix4x4::scaling(0.1f, 0.1f, 0.1f));

    Mesh venusMesh;
    venusMesh.createSphere(16, 16, Color(255, 200, 200));
    venusMesh.setModelMatrix(Matrix4x4::scaling(0.2f, 0.2f, 0.2f));

    Mesh earthMesh;
    earthMesh.createSphere(16, 16, Color(0, 0, 255));
    earthMesh.setModelMatrix(Matrix4x4::scaling(0.2f, 0.2f, 0.2f));

    Mesh marsMesh;
    marsMesh.createSphere(16, 16, Color(255, 0, 0));
    marsMesh.setModelMatrix(Matrix4x4::scaling(0.2f, 0.2f, 0.2f));

    Mesh jupiterMesh;
    jupiterMesh.createSphere(16, 16, Color(255, 200, 0));
    jupiterMesh.setModelMatrix(Matrix4x4::scaling(0.5f, 0.5f, 0.5f));

    Mesh saturnMesh;
    saturnMesh.createSphere(16, 16, Color(255, 200, 0));
    saturnMesh.setModelMatrix(Matrix4x4::scaling(0.5f, 0.5f, 0.5f));

    Mesh uranusMesh;
    uranusMesh.createSphere(16, 16, Color(0, 255, 255));
    uranusMesh.setModelMatrix(Matrix4x4::scaling(0.3f, 0.3f, 0.3f));

    Mesh neptuneMesh;
    neptuneMesh.createSphere(16, 16, Color(0, 0, 255));
    neptuneMesh.setModelMatrix(Matrix4x4::scaling(0.3f, 0.3f, 0.3f));

    float rotationSun = 0.0f;
    float rotationMercury = 0.0f;
    float rotationVenus = 0.0f;
    float rotationEarth = 0.0f;
    float rotationMars = 0.0f;
    float rotationJupiter = 0.0f;
    float rotationSaturn = 0.0f;
    float rotationUranus = 0.0f;
    float rotationNeptune = 0.0f;

    rasterizer.getCurrentShader()->setCameraPosition(Vec3(0.0f, 5.0f, 5.0f));
    rasterizer.getCurrentShader()->setViewMatrix(Matrix4x4::lookAt(Vec3(0.0f, 5.0f, 5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f)));
    rasterizer.getCurrentShader()->setProjectionMatrix(Matrix4x4::perspective(60.0f * (3.14159f / 180.0f), static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 100.0f));

    uint32_t lastTick = SDL_GetTicks();
    while (!rasterizer.shouldQuit()) {
        rasterizer.handleEvents();

        uint32_t currentTick = SDL_GetTicks();
        float deltaTime = (currentTick - lastTick) / 1000.0f;
        lastTick = currentTick;

        rotationSun += 0.1f * deltaTime;
        rotationMercury += 0.2f * deltaTime;
        rotationVenus += 0.3f * deltaTime;
        rotationEarth += 0.4f * deltaTime;
        rotationMars += 0.5f * deltaTime;
        rotationJupiter += 0.6f * deltaTime;
        rotationSaturn += 0.7f * deltaTime;
        rotationUranus += 0.8f * deltaTime;
        rotationNeptune += 0.9f * deltaTime;

        sunMesh.setModelMatrix(Matrix4x4::rotationY(rotationSun) * Matrix4x4::translation(0.0f, 0.0f, 0.0f));
        mercuryMesh.setModelMatrix(Matrix4x4::rotationY(rotationMercury) * Matrix4x4::translation(1.0f, 0.0f, 0.0f) * Matrix4x4::scaling(0.1f, 0.1f, 0.1f));
        venusMesh.setModelMatrix(Matrix4x4::rotationY(rotationVenus) * Matrix4x4::translation(1.5f, 0.0f, 0.0f) * Matrix4x4::scaling(0.2f, 0.2f, 0.2f));
        earthMesh.setModelMatrix(Matrix4x4::rotationY(rotationEarth) * Matrix4x4::translation(2.0f, 0.0f, 0.0f) * Matrix4x4::scaling(0.2f, 0.2f, 0.2f));
        marsMesh.setModelMatrix(Matrix4x4::rotationY(rotationMars) * Matrix4x4::translation(2.5f, 0.0f, 0.0f) * Matrix4x4::scaling(0.2f, 0.2f, 0.2f));
        jupiterMesh.setModelMatrix(Matrix4x4::rotationY(rotationJupiter) * Matrix4x4::translation(3.0f, 0.0f, 0.0f) * Matrix4x4::scaling(0.5f, 0.5f, 0.5f));
        saturnMesh.setModelMatrix(Matrix4x4::rotationY(rotationSaturn) * Matrix4x4::translation(3.5f, 0.0f, 0.0f) * Matrix4x4::scaling(0.5f, 0.5f, 0.5f));
        uranusMesh.setModelMatrix(Matrix4x4::rotationY(rotationUranus) * Matrix4x4::translation(4.0f, 0.0f, 0.0f) * Matrix4x4::scaling(0.3f, 0.3f, 0.3f));
        neptuneMesh.setModelMatrix(Matrix4x4::rotationY(rotationNeptune) * Matrix4x4::translation(4.5f, 0.0f, 0.0f) * Matrix4x4::scaling(0.3f, 0.3f, 0.3f));

        rasterizer.clear(Color(20, 20, 20));

        rasterizer.renderMesh(sunMesh, *rasterizer.getCurrentShader());
        rasterizer.renderMesh(mercuryMesh, *rasterizer.getCurrentShader());
        rasterizer.renderMesh(venusMesh, *rasterizer.getCurrentShader());
        rasterizer.renderMesh(earthMesh, *rasterizer.getCurrentShader());
        rasterizer.renderMesh(marsMesh, *rasterizer.getCurrentShader());
        rasterizer.renderMesh(jupiterMesh, *rasterizer.getCurrentShader());
        rasterizer.renderMesh(saturnMesh, *rasterizer.getCurrentShader());
        rasterizer.renderMesh(uranusMesh, *rasterizer.getCurrentShader());
        rasterizer.renderMesh(neptuneMesh, *rasterizer.getCurrentShader());

        rasterizer.present();
    }
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

    // scene_1(rasterizer);
    // scene_2(rasterizer);
    // scene_3(rasterizer);
    // scene_4(rasterizer);

    LOG_INFO("Shutting down application");
    SDL_Quit();
    return 0;
}
