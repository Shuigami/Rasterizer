#include <iostream>
#include <SDL.h>
#include "rasterizer.h"
#include "mesh.h"
#include "shader.h"
#include "camera.h"
#include "vector.h"
#include "matrix.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
bool wireframeMode = false;

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    Rasterizer rasterizer(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!rasterizer.initialize()) {
        std::cerr << "Failed to initialize rasterizer." << std::endl;
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

    Mesh cubeMesh;
    cubeMesh.createCube(Color(80, 80, 80));

    Mesh sphereMesh;
    sphereMesh.createSphere(16, 16, Color(50, 50, 200));

    Mesh wellMesh;
    wellMesh.loadFromOBJ("assets/moto.obj");

    Mesh planeMesh;
    planeMesh.createPlane(1.0f, 1.0f, Color(80, 80, 80));

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

    Light pointLight;
    pointLight.type = Light::Type::Point;
    pointLight.position = Vec3(0.0f, 2.0f, 0.0f);
    pointLight.color = Color(255, 255, 255);
    pointLight.intensity = 1.f;
    pointLight.range = 20.0f;
    phongShader.addLight(pointLight);
    toonShader.addLight(pointLight);

    bool running = true;
    float rotation = 0.0f;
    uint32_t lastTick = SDL_GetTicks();

    while (running && !rasterizer.shouldQuit()) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                } else if (event.key.keysym.sym == SDLK_w) {
                    wireframeMode = !wireframeMode;
                    std::cout << "Wireframe mode: " << (wireframeMode ? "ON" : "OFF") << std::endl;
                }
            }
        }

        Vec3 cameraPos = camera.getPosition();

        uint32_t currentTick = SDL_GetTicks();
        float deltaTime = (currentTick - lastTick) / 1000.0f;
        lastTick = currentTick;

        rotation += 0.7f * deltaTime;

        phongShader.setCameraPosition(cameraPos);
        flatShader.setCameraPosition(cameraPos);
        toonShader.setCameraPosition(cameraPos);

        pointLight.position = Vec3(
            5.0f * std::cos(rotation),
            2.0f,
            5.0f * std::sin(rotation)
        );

        phongShader.clearLights();
        toonShader.clearLights();
        phongShader.addLight(pointLight);
        toonShader.addLight(pointLight);

        Matrix4x4 cubeModelMatrix = Matrix4x4::translation(0.0f, 0.5f, 0.0f);
        Matrix4x4 sphereModelMatrix = Matrix4x4::translation(0.0f, 0.0f, 0.0f);
        Matrix4x4 wellModelMatrix = Matrix4x4::translation(0.0f, -0.5f, 0.0f) * Matrix4x4::rotationY(M_PI / 4) * Matrix4x4::scaling(0.3f, 0.3f, 0.3f);
        Matrix4x4 planeModelMatrix = Matrix4x4::translation(0.0f, -0.5f, 0.0f) * Matrix4x4::scaling(1.0f, 1.0f, 11.0f);

        phongShader.setViewMatrix(camera.getViewMatrix());
        phongShader.setProjectionMatrix(camera.getProjectionMatrix());
        flatShader.setViewMatrix(camera.getViewMatrix());
        flatShader.setProjectionMatrix(camera.getProjectionMatrix());
        toonShader.setViewMatrix(camera.getViewMatrix());
        toonShader.setProjectionMatrix(camera.getProjectionMatrix());

        rasterizer.clear(Color(20, 20, 20));

        Shader* currentShader = &phongShader;

        // currentShader->setModelMatrix(cubeModelMatrix);
        // rasterizer.renderMesh(cubeMesh, *currentShader, wireframeMode);

        // currentShader->setModelMatrix(sphereModelMatrix);
        // rasterizer.renderMesh(sphereMesh, *currentShader, wireframeMode);

        // currentShader->setModelMatrix(wellModelMatrix);
        // rasterizer.renderMesh(wellMesh, *currentShader, wireframeMode);
        
        currentShader->setModelMatrix(planeModelMatrix);
        rasterizer.renderMesh(planeMesh, *currentShader, wireframeMode);

        rasterizer.present();

        SDL_Delay(16);
    }

    SDL_Quit();
    return 0;
}
