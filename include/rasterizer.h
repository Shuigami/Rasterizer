#pragma once

#include <SDL.h>
#include <vector>
#include "vector.h"
#include "mesh.h"
#include "shader.h"
#include "logger.h"

class Rasterizer {
public:
    Rasterizer(int width, int height);
    ~Rasterizer();

    bool initialize();
    void clear(const Color& color);
    void drawPoint(int x, int y, const Color& color);
    void drawLine(int x1, int y1, int x2, int y2, const Color& color);
    void drawTriangle(const Vec4& v1, const Vec4& v2, const Vec4& v3, const Color& color);
    void fillTriangle(const Vec4& v1, const Vec4& v2, const Vec4& v3, const Color& color);
    void renderMesh(const Mesh& mesh, const Shader& shader);
    void renderShadowMap(const Mesh& mesh, const Shader& shader);
    void present();
    bool shouldQuit() const;
    void handleEvents();
    
    void beginShadowPass();
    float getShadowFactor(const Vec3& worldPos) const;

    Shader* getCurrentShader() const;
    void setCurrentShader(int index);
    void addShader(Shader* shader);
    int getShaderIndex() const { return m_shaderIndex; }
    int getShaderCount() const { return static_cast<int>(m_shaders.size()); }
    Shader* getShader(int index) const { return m_shaders[index]; }
    bool isShadowsEnabled() const { return m_shadowsEnabled; }
    void setShadowsEnabled(bool enabled);
    bool isWireframeMode() const { return m_wireframeMode; }
    void setWireframeMode(bool enabled);

    // Scene index asked for with keys 1-9 since the last call, or -1.
    int takeSceneRequest();

    // Mouse drag (pixels), wheel steps and R key since the last call.
    struct ViewInput {
        float dragX = 0.0f;
        float dragY = 0.0f;
        float wheel = 0.0f;
        bool reset = false;
    };
    ViewInput takeViewInput();

private:
    int m_width;
    int m_height;
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    SDL_Texture* m_frameBuffer;
    std::vector<uint32_t> m_colorBuffer;
    std::vector<float> m_depthBuffer;

    int m_shaderIndex;
    std::vector<Shader*> m_shaders;
    
#ifdef __EMSCRIPTEN__
    // 8 lights x 2048^2 floats would reserve 128 MB in the browser.
    static const int SHADOW_MAP_SIZE = 1024;
#else
    static const int SHADOW_MAP_SIZE = 2048;
#endif
    static const int MAX_LIGHTS = 8;
    struct LightData {
        std::vector<float> shadowMap;
        Matrix4x4 viewMatrix;
        Matrix4x4 projectionMatrix;
        Matrix4x4 shadowMatrix;
    };
    std::vector<LightData> m_lightData;
    bool m_shadowsEnabled;
    
    bool m_quit;
    bool m_wireframeMode;
    int m_sceneRequest;
    bool m_dragging = false;
    ViewInput m_viewInput;

    Vec4 viewportTransform(const Vec4& clipCoords) const;
    bool isInsideFrustum(const Vec4& clipCoords) const;
    bool isInsidePlane(const Vec4& position, int planeIndex, int sign);
    float intersectionParameter(const Vec4& v1, const Vec4& v2, int planeIndex, int sign);
};
