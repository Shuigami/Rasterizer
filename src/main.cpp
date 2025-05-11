#include <iostream>
#include <memory>
#include <chrono>
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
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create rasterizer
    Rasterizer rasterizer(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!rasterizer.initialize()) {
        std::cerr << "Failed to initialize rasterizer." << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create camera
    Camera camera(
        Vec3(0.0f, 0.0f, 5.0f),  // Position
        Vec3(0.0f, 0.0f, 0.0f),  // Target
        Vec3(0.0f, 1.0f, 0.0f),  // Up
        60.0f * (3.14159f / 180.0f),  // FOV in radians
        static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT,  // Aspect ratio
        0.1f,  // Near plane
        100.0f  // Far plane
    );

    // Create a cube mesh
    Mesh cubeMesh;
    cubeMesh.createCube();

    // Create a sphere mesh
    Mesh sphereMesh;
    sphereMesh.createSphere(16, 16);

    // Create shaders
    PhongShader phongShader;
    phongShader.setAmbient(0.2f);
    phongShader.setDiffuse(0.7f);
    phongShader.setSpecular(0.5f);
    phongShader.setShininess(32.0f);

    FlatShader flatShader(Color(200, 50, 50));

    // Create a texture (checkerboard pattern)
    std::shared_ptr<Texture> checkerTexture = std::make_shared<Texture>();
    checkerTexture->create(256, 256);
    for (int y = 0; y < 256; y++) {
        for (int x = 0; x < 256; x++) {
            bool isWhite = ((x / 32) % 2 == 0) ^ ((y / 32) % 2 == 0);
            checkerTexture->setPixel(x, y, isWhite ? Color(255, 255, 255) : Color(0, 0, 0));
        }
    }

    // Create a texture shader and set the texture
    TextureShader textureShader;
    textureShader.setTexture(checkerTexture);

    // Set up point light
    Light pointLight;
    pointLight.type = Light::Type::Point;
    pointLight.position = Vec3(0.0f, 2.0f, 2.0f);
    pointLight.color = Color(255, 255, 255);
    pointLight.intensity = 1.f;
    pointLight.range = 10.0f;
    phongShader.addLight(pointLight);

    // Main game loop
    bool running = true;
    float rotation = 0.0f;
    uint32_t lastTick = SDL_GetTicks();

    while (running && !rasterizer.shouldQuit()) {
        // Handle events
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

        // Set camera position to control point of view
        // This affects backface culling which depends on the camera position
        Vec3 cameraPos = camera.getPosition();

        // Calculate delta time
        uint32_t currentTick = SDL_GetTicks();
        float deltaTime = (currentTick - lastTick) / 1000.0f;
        lastTick = currentTick;

        // Update rotation (reduced speed to better see the effect)
        rotation += 0.7f * deltaTime;

        // Update camera position for all shaders
        phongShader.setCameraPosition(cameraPos);
        flatShader.setCameraPosition(cameraPos);
        textureShader.setCameraPosition(cameraPos);

        // Update matrices
        Matrix4x4 rotationMatrix = Matrix4x4::rotationY(rotation);
        Matrix4x4 cubeModelMatrix = rotationMatrix * Matrix4x4::translation(0.0f, 0.0f, 0.0f);
        Matrix4x4 sphereModelMatrix = rotationMatrix * Matrix4x4::translation(1.5f, 0.0f, 0.0f);

        // Set the view and projection matrices for the shaders
        phongShader.setViewMatrix(camera.getViewMatrix());
        phongShader.setProjectionMatrix(camera.getProjectionMatrix());
        flatShader.setViewMatrix(camera.getViewMatrix());
        flatShader.setProjectionMatrix(camera.getProjectionMatrix());
        textureShader.setViewMatrix(camera.getViewMatrix());
        textureShader.setProjectionMatrix(camera.getProjectionMatrix());

        // Clear the screen
        rasterizer.clear(Color(50, 50, 50));

        // Render the cube with the phong shader
        phongShader.setModelMatrix(cubeModelMatrix);
        rasterizer.renderMesh(cubeMesh, cubeModelMatrix, phongShader, wireframeMode);

        // Render the sphere with the texture shader
        phongShader.setModelMatrix(sphereModelMatrix);
        rasterizer.renderMesh(sphereMesh, sphereModelMatrix, phongShader, wireframeMode);

        // Present the frame buffer
        rasterizer.present();

        // Cap the frame rate
        SDL_Delay(16); // Approximately 60 FPS
    }

    // Clean up and exit
    SDL_Quit();
    return 0;
}
