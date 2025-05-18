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

private:
    int m_width;
    int m_height;
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    SDL_Texture* m_frameBuffer;
    std::vector<uint32_t> m_colorBuffer;
    std::vector<float> m_depthBuffer;
    
    static const int SHADOW_MAP_SIZE = 2048; 
    std::vector<float> m_shadowMap;
    Matrix4x4 m_lightViewMatrix;
    Matrix4x4 m_lightProjectionMatrix;
    Matrix4x4 m_shadowMatrix;
    bool m_shadowsEnabled;
    
    bool m_quit;
    bool m_wireframeMode;

    Vec4 viewportTransform(const Vec4& clipCoords) const;
    bool isInsideFrustum(const Vec4& clipCoords) const;
    bool isInsidePlane(const Vec4& position, int planeIndex, int sign);
    float intersectionParameter(const Vec4& v1, const Vec4& v2, int planeIndex, int sign);
};
