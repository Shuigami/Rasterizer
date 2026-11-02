#include <SDL.h>
#include "rasterizer.h"
#include "mesh.h"
#include "shader.h"
#include "camera.h"
#include "vector.h"
#include "matrix.h"
#include <logger.h>
#include "gallery_scenes.h"
#include <algorithm>
#include <chrono>
#include <string>
#include <cmath>
#include <memory>
#include <vector>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
// A software rasterizer runs on one CPU core in the browser: render fewer pixels.
const int WINDOW_WIDTH = 960;
const int WINDOW_HEIGHT = 540;
#else
const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
#endif

const float PI = 3.14159f;

void load_scene(Shader& shader) {
    Camera camera(
        Vec3(0.0f, 1.0f, 5.0f),
        Vec3(0.0f, 1.0f, 0.0f),
        Vec3(0.0f, 1.0f, 0.0f),
        60.0f * (PI / 180.0f),
        static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT,
        0.1f,
        100.0f
    );

    shader.clearLights();

    Light pointLight;
    pointLight.type = Light::Type::Point;
    pointLight.position = Vec3(2.0f, 2.0f, 2.0f);
    pointLight.color = Color(255, 255, 255);
    pointLight.intensity = 1.2f;
    pointLight.range = 20.0f;

    shader.addLight(pointLight);

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

    rasterizer.setCurrentShader(0);
    rasterizer.setShadowsEnabled(false);
    LOG_INFO("Shaders loaded successfully");
}

void configure_phong(Rasterizer& rasterizer, float ambient, float diffuse, float specular, float shininess) {
    if (auto* phong = dynamic_cast<PhongShader*>(rasterizer.getShader(0))) {
        phong->setAmbient(ambient);
        phong->setDiffuse(diffuse);
        phong->setSpecular(specular);
        phong->setShininess(shininess);
    }
}

// Every scene starts from the default camera and light on all shaders, so
// switching shaders or scenes never keeps state from a previous scene.
void reset_shaders(Rasterizer& rasterizer) {
    for (int i = 0; i < rasterizer.getShaderCount(); ++i) {
        load_scene(*rasterizer.getShader(i));
    }
    configure_phong(rasterizer, 0.2f, 0.7f, 0.5f, 32.0f);
}

// A scene owns its meshes and draws one frame per update(). Static scenes
// are only redrawn when something changes, and can be orbited with the mouse.
class Scene {
public:
    virtual ~Scene() = default;
    virtual void update(Rasterizer& rasterizer, float deltaTime) = 0;
    virtual bool animated() const { return true; }
    virtual void view(Rasterizer&, const Rasterizer::ViewInput&) {}
};

// Gallery scenes (see gallery_scenes.cpp): static, lit by a warm key light
// and a cool fill light, viewed through an orbiting 40-degree camera.
class GalleryView : public Scene {
public:
    GalleryView(Rasterizer& rasterizer, const std::string& name)
        : m_scene(makeGalleryScene(name)) {
        Vec3 offset = m_scene.eye - m_scene.target;
        m_initialDistance = offset.length();
        m_initialPitch = std::asin(offset.y / m_initialDistance);
        m_initialYaw = std::atan2(offset.z, offset.x);
        resetView();

        Light key;
        key.type = Light::Type::Directional;
        key.direction = Vec3(0.7f, -1.0f, -0.6f);
        key.intensity = 0.85f;
        key.color = Color(255, 242, 222);
        key.range = 100.0f;

        Light fill;
        fill.type = Light::Type::Directional;
        fill.direction = Vec3(-1.0f, -0.3f, 0.5f);
        fill.intensity = 0.24f;
        fill.color = Color(200, 221, 255);
        fill.range = 100.0f;

        const Matrix4x4 projection = Matrix4x4::perspective(
            GALLERY_FOV_DEGREES * (PI / 180.0f),
            static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 100.0f);
        for (int i = 0; i < rasterizer.getShaderCount(); ++i) {
            Shader& shader = *rasterizer.getShader(i);
            shader.clearLights();
            shader.addLight(key);
            shader.addLight(fill);
            shader.setProjectionMatrix(projection);
        }
        configure_phong(rasterizer, m_scene.ambient, 0.65f, 0.16f, 48.0f);
        applyCamera(rasterizer);
    }

    bool animated() const override { return false; }

    void view(Rasterizer& rasterizer, const Rasterizer::ViewInput& input) override {
        if (input.reset) {
            resetView();
        }
        const float maxPitch = 1.45f;
        m_yaw -= input.dragX * 0.008f;
        m_pitch = std::max(0.05f, std::min(maxPitch, m_pitch + input.dragY * 0.008f));
        m_distance *= std::pow(0.9f, input.wheel);
        m_distance = std::max(0.5f * m_initialDistance, std::min(2.0f * m_initialDistance, m_distance));
        applyCamera(rasterizer);
    }

    void update(Rasterizer& rasterizer, float) override {
        rasterizer.clear(m_scene.background);
        Shader& shader = *rasterizer.getCurrentShader();
        for (const Mesh& mesh : m_scene.meshes) {
            rasterizer.renderMesh(mesh, shader);
        }
    }

private:
    void resetView() {
        m_yaw = m_initialYaw;
        m_pitch = m_initialPitch;
        m_distance = m_initialDistance;
    }

    void applyCamera(Rasterizer& rasterizer) {
        const Vec3 eye = m_scene.target + Vec3(
            m_distance * std::cos(m_pitch) * std::cos(m_yaw),
            m_distance * std::sin(m_pitch),
            m_distance * std::cos(m_pitch) * std::sin(m_yaw));
        const Matrix4x4 view = Matrix4x4::lookAt(eye, m_scene.target, Vec3(0.0f, 1.0f, 0.0f));
        for (int i = 0; i < rasterizer.getShaderCount(); ++i) {
            Shader& shader = *rasterizer.getShader(i);
            shader.setCameraPosition(eye);
            shader.setViewMatrix(view);
        }
    }

    GalleryScene m_scene;
    float m_initialYaw = 0.0f, m_initialPitch = 0.0f, m_initialDistance = 1.0f;
    float m_yaw = 0.0f, m_pitch = 0.0f, m_distance = 1.0f;
};

// Scene 1: a sphere lit by a light orbiting around it.
class OrbitingLightScene : public Scene {
public:
    OrbitingLightScene() {
        m_sphere.createSphere(16, 16, Color(50, 50, 200));
        m_light.type = Light::Type::Point;
        m_light.position = Vec3(2.0f, 2.0f, 2.0f);
        m_light.color = Color(255, 255, 255);
        m_light.intensity = 1.2f;
        m_light.range = 20.0f;
    }

    void update(Rasterizer& rasterizer, float deltaTime) override {
        m_rotation += 0.7f * deltaTime;
        m_light.position = Vec3(2.0f * std::cos(m_rotation), 2.0f, 2.0f * std::sin(m_rotation));

        Shader& shader = *rasterizer.getCurrentShader();
        shader.clearLights();
        shader.addLight(m_light);

        rasterizer.clear(Color(20, 20, 20));
        rasterizer.beginShadowPass();
        rasterizer.renderShadowMap(m_sphere, shader);
        rasterizer.renderMesh(m_sphere, shader);
    }

private:
    Mesh m_sphere;
    Light m_light;
    float m_rotation = 0.0f;
};

// Scene 2: a sphere circling above a plane, casting a shadow on it.
class ShadowScene : public Scene {
public:
    ShadowScene() {
        m_sphere.createSphere(16, 16, Color(50, 50, 200));
        m_plane.createPlane(5.0f, 5.0f, Color(255, 0, 0));
        m_plane.setModelMatrix(Matrix4x4::translation(0.0f, -0.5f, 0.0f));
    }

    void update(Rasterizer& rasterizer, float deltaTime) override {
        m_rotation += 0.7f * deltaTime;
        m_sphere.setModelMatrix(Matrix4x4::rotationY(m_rotation) * Matrix4x4::translation(1.0f, 0.0f, 0.0f));

        Shader& shader = *rasterizer.getCurrentShader();
        rasterizer.clear(Color(20, 20, 20));
        rasterizer.beginShadowPass();
        rasterizer.renderShadowMap(m_plane, shader);
        rasterizer.renderShadowMap(m_sphere, shader);
        rasterizer.renderMesh(m_plane, shader);
        rasterizer.renderMesh(m_sphere, shader);
    }

private:
    Mesh m_sphere;
    Mesh m_plane;
    float m_rotation = 0.0f;
};

// Scene 3: the well model loaded from assets/well.obj, turning on itself.
class WellScene : public Scene {
public:
    WellScene() {
        m_well.loadFromOBJ("assets/well.obj");
        m_well.setModelMatrix(Matrix4x4::scaling(0.1f, 0.1f, 0.1f));
    }

    void update(Rasterizer& rasterizer, float deltaTime) override {
        m_rotation += 0.7f * deltaTime;
        m_well.setModelMatrix(Matrix4x4::rotationY(m_rotation) * Matrix4x4::translation(0.0f, -1.0f, 0.0f) * Matrix4x4::scaling(0.1f, 0.1f, 0.1f));

        rasterizer.clear(Color(20, 20, 20));
        rasterizer.renderMesh(m_well, *rasterizer.getCurrentShader());
    }

private:
    Mesh m_well;
    float m_rotation = 0.0f;
};

// Scene 4: a solar system seen from above.
class SolarSystemScene : public Scene {
public:
    SolarSystemScene(Rasterizer& rasterizer) {
        const Body bodies[] = {
            {Color(255, 255, 0), 0.0f, 1.0f, 0.1f},
            {Color(150, 150, 150), 1.0f, 0.1f, 0.2f},
            {Color(255, 200, 200), 1.5f, 0.2f, 0.3f},
            {Color(0, 0, 255), 2.0f, 0.2f, 0.4f},
            {Color(255, 0, 0), 2.5f, 0.2f, 0.5f},
            {Color(255, 200, 0), 3.0f, 0.5f, 0.6f},
            {Color(255, 200, 0), 3.5f, 0.5f, 0.7f},
            {Color(0, 255, 255), 4.0f, 0.3f, 0.8f},
            {Color(0, 0, 255), 4.5f, 0.3f, 0.9f},
        };
        for (const Body& body : bodies) {
            m_bodies.push_back(body);
            m_meshes.emplace_back();
            m_meshes.back().createSphere(16, 16, body.color);
            m_rotations.push_back(0.0f);
        }

        const Vec3 eye(0.0f, 5.0f, 5.0f);
        const Matrix4x4 view = Matrix4x4::lookAt(eye, Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
        const Matrix4x4 projection = Matrix4x4::perspective(60.0f * (PI / 180.0f), static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 100.0f);
        for (int i = 0; i < rasterizer.getShaderCount(); ++i) {
            Shader& shader = *rasterizer.getShader(i);
            shader.setCameraPosition(eye);
            shader.setViewMatrix(view);
            shader.setProjectionMatrix(projection);
        }
    }

    void update(Rasterizer& rasterizer, float deltaTime) override {
        rasterizer.clear(Color(20, 20, 20));
        for (size_t i = 0; i < m_meshes.size(); ++i) {
            const Body& body = m_bodies[i];
            m_rotations[i] += body.speed * deltaTime;
            m_meshes[i].setModelMatrix(
                Matrix4x4::rotationY(m_rotations[i]) *
                Matrix4x4::translation(body.distance, 0.0f, 0.0f) *
                Matrix4x4::scaling(body.scale, body.scale, body.scale));
            rasterizer.renderMesh(m_meshes[i], *rasterizer.getCurrentShader());
        }
    }

private:
    struct Body {
        Color color;
        float distance;
        float scale;
        float speed;
    };
    std::vector<Body> m_bodies;
    std::vector<Mesh> m_meshes;
    std::vector<float> m_rotations;
};

// Keys 1-5: gallery scenes; keys 6-9: the animated demos.
const char* const GALLERY_SCENES[] = {"studio", "village", "temple", "jardin", "orbites"};
const int GALLERY_COUNT = 5;
const int SCENE_COUNT = GALLERY_COUNT + 4;

struct App {
    std::unique_ptr<Rasterizer> rasterizer;
    std::unique_ptr<Scene> scene;
    int sceneIndex = -1;
    uint32_t lastTick = 0;
    bool dirty = true;
    int lastSettings = -1;
    float renderMs = 0.0f;
};

App app;

void load_scene_index(int index) {
    if (index < 0 || index >= SCENE_COUNT) {
        return;
    }
    Rasterizer& rasterizer = *app.rasterizer;
    reset_shaders(rasterizer);
    if (index < GALLERY_COUNT) {
        app.scene = std::make_unique<GalleryView>(rasterizer, GALLERY_SCENES[index]);
    } else {
        switch (index - GALLERY_COUNT) {
            case 0: app.scene = std::make_unique<OrbitingLightScene>(); break;
            case 1: app.scene = std::make_unique<ShadowScene>(); break;
            case 2: app.scene = std::make_unique<WellScene>(); break;
            default: app.scene = std::make_unique<SolarSystemScene>(rasterizer); break;
        }
    }
    app.sceneIndex = index;
    app.lastTick = SDL_GetTicks();
    app.dirty = true;
    LOG_INFO("Scene " + std::to_string(index + 1));
}

void frame() {
    Rasterizer& rasterizer = *app.rasterizer;
    rasterizer.handleEvents();
    int request = rasterizer.takeSceneRequest();
    if (request >= 0) {
        load_scene_index(request);
    }

    Rasterizer::ViewInput input = rasterizer.takeViewInput();
    if (input.dragX != 0.0f || input.dragY != 0.0f || input.wheel != 0.0f || input.reset) {
        app.scene->view(rasterizer, input);
        app.dirty = true;
    }

    // Shader, wireframe or shadow changes (keys or page buttons) need a redraw.
    int settings = rasterizer.getShaderIndex() * 4
        + (rasterizer.isWireframeMode() ? 2 : 0) + (rasterizer.isShadowsEnabled() ? 1 : 0);
    if (settings != app.lastSettings) {
        app.lastSettings = settings;
        app.dirty = true;
    }

    uint32_t currentTick = SDL_GetTicks();
    float deltaTime = (currentTick - app.lastTick) / 1000.0f;
    app.lastTick = currentTick;

    if (!app.scene->animated() && !app.dirty) {
        return;
    }
    auto start = std::chrono::steady_clock::now();
    app.scene->update(rasterizer, deltaTime);
    app.renderMs = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - start).count();
    rasterizer.present();
    app.dirty = false;
}

#ifdef __EMSCRIPTEN__
// Controls for the web page (see web/shell.html). The page may call them
// before main() has finished setting up, so each one checks for a scene.
static bool ready() { return app.rasterizer && app.scene; }

extern "C" {
EMSCRIPTEN_KEEPALIVE void rz_set_scene(int index) { if (ready()) load_scene_index(index); }
EMSCRIPTEN_KEEPALIVE void rz_set_shader(int index) { if (ready()) app.rasterizer->setCurrentShader(index); }
EMSCRIPTEN_KEEPALIVE void rz_set_shadows(int enabled) { if (ready()) app.rasterizer->setShadowsEnabled(enabled != 0); }
EMSCRIPTEN_KEEPALIVE void rz_set_wireframe(int enabled) { if (ready()) app.rasterizer->setWireframeMode(enabled != 0); }
EMSCRIPTEN_KEEPALIVE int rz_scene() { return ready() ? app.sceneIndex : -1; }
EMSCRIPTEN_KEEPALIVE int rz_shader() { return ready() ? app.rasterizer->getShaderIndex() : -1; }
EMSCRIPTEN_KEEPALIVE int rz_shadows() { return ready() && app.rasterizer->isShadowsEnabled() ? 1 : 0; }
EMSCRIPTEN_KEEPALIVE int rz_wireframe() { return ready() && app.rasterizer->isWireframeMode() ? 1 : 0; }
EMSCRIPTEN_KEEPALIVE void rz_reset_view() {
    if (ready()) {
        Rasterizer::ViewInput input;
        input.reset = true;
        app.scene->view(*app.rasterizer, input);
        app.dirty = true;
    }
}
EMSCRIPTEN_KEEPALIVE float rz_render_ms() { return app.renderMs; }
}
#endif

int main() {
    Logger& logger = Logger::getInstance();
    logger.setLevel(LogLevel::INFO);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        LOG_ERROR("SDL initialization failed: " + std::string(SDL_GetError()));
        return 1;
    }

    LOG_INFO("Starting rasterizer...");
    app.rasterizer = std::make_unique<Rasterizer>(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!app.rasterizer->initialize()) {
        LOG_ERROR("Failed to initialize rasterizer.");
        SDL_Quit();
        return 1;
    }
    load_shaders(*app.rasterizer);

    load_scene_index(0);

#ifdef __EMSCRIPTEN__
    // The browser drives the loop: one call to frame() per animation frame.
    emscripten_set_main_loop(frame, 0, 1);
#else
    while (!app.rasterizer->shouldQuit()) {
        frame();
    }

    LOG_INFO("Shutting down application");
    app.scene.reset();
    app.rasterizer.reset();
    SDL_Quit();
#endif
    return 0;
}
