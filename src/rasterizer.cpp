#include "rasterizer.h"
#include "logger.h"
#include <algorithm>
#include <iostream>

struct VertexWithAttributes {
    Vec4 position;
    VertexShaderOutput attributes;
    
    VertexWithAttributes() {}
    
    VertexWithAttributes(const Vec4& pos, const VertexShaderOutput& attr) 
        : position(pos), attributes(attr) {}
};

Rasterizer::Rasterizer(int width, int height)
    : m_width(width), m_height(height), m_window(nullptr), m_renderer(nullptr),
      m_frameBuffer(nullptr), m_quit(false), m_shadowsEnabled(true), m_wireframeMode(false) {

    m_colorBuffer.resize(width * height, 0);
    m_depthBuffer.resize(width * height, 1.0f);
    
    m_shadowMap.resize(SHADOW_MAP_SIZE * SHADOW_MAP_SIZE, 1.0f);
}

Rasterizer::~Rasterizer() {
    if (m_frameBuffer) {
        SDL_DestroyTexture(m_frameBuffer);
    }
    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
    }
}

bool Rasterizer::initialize() {
    m_window = SDL_CreateWindow(
        "Software Rasterizer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        m_width, m_height,
        SDL_WINDOW_SHOWN
    );

    if (!m_window) {
        LOG_ERROR("Failed to create window: " + std::string(SDL_GetError()));
        return false;
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (!m_renderer) {
        LOG_ERROR("Failed to create renderer: " + std::string(SDL_GetError()));
        return false;
    }

    m_frameBuffer = SDL_CreateTexture(
        m_renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        m_width, m_height
    );

    if (!m_frameBuffer) {
        LOG_ERROR("Failed to create frame buffer: " + std::string(SDL_GetError()));
        return false;
    }

    LOG_INFO("Rasterizer initialized successfully");
    return true;
}

void Rasterizer::clear(const Color& color) {
    uint32_t clearColor = color.toUint32();
    std::fill(m_colorBuffer.begin(), m_colorBuffer.end(), clearColor);

    std::fill(m_depthBuffer.begin(), m_depthBuffer.end(), 1.0f);
}

void Rasterizer::drawPoint(int x, int y, const Color& color) {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return;
    }

    m_colorBuffer[y * m_width + x] = color.toUint32();
}

void Rasterizer::drawLine(int x1, int y1, int x2, int y2, const Color& color) {
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        drawPoint(x1, y1, color);

        if (x1 == x2 && y1 == y2) {
            break;
        }

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void Rasterizer::drawTriangle(const Vec4& v1, const Vec4& v2, const Vec4& v3, const Color& color) {
    drawLine(
        static_cast<int>(v1.x), static_cast<int>(v1.y),
        static_cast<int>(v2.x), static_cast<int>(v2.y),
        color
    );

    drawLine(
        static_cast<int>(v2.x), static_cast<int>(v2.y),
        static_cast<int>(v3.x), static_cast<int>(v3.y),
        color
    );

    drawLine(
        static_cast<int>(v3.x), static_cast<int>(v3.y),
        static_cast<int>(v1.x), static_cast<int>(v1.y),
        color
    );
}

void Rasterizer::fillTriangle(const Vec4& v1, const Vec4& v2, const Vec4& v3, const Color& color) {
    int minX = std::max(0, std::min(std::min(static_cast<int>(v1.x), static_cast<int>(v2.x)), static_cast<int>(v3.x)));
    int maxX = std::min(m_width - 1, std::max(std::max(static_cast<int>(v1.x), static_cast<int>(v2.x)), static_cast<int>(v3.x)));
    int minY = std::max(0, std::min(std::min(static_cast<int>(v1.y), static_cast<int>(v2.y)), static_cast<int>(v3.y)));
    int maxY = std::min(m_height - 1, std::max(std::max(static_cast<int>(v1.y), static_cast<int>(v2.y)), static_cast<int>(v3.y)));

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            Vec2 p(static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f);

            Vec2 a(v1.x, v1.y);
            Vec2 b(v2.x, v2.y);
            Vec2 c(v3.x, v3.y);

            Vec2 v0 = b - a;
            Vec2 v1 = c - a;
            Vec2 v2 = p - a;

            float d00 = v0.dot(v0);
            float d01 = v0.dot(v1);
            float d11 = v1.dot(v1);
            float d20 = v2.dot(v0);
            float d21 = v2.dot(v1);

            float denom = d00 * d11 - d01 * d01;
            if (std::abs(denom) < 1e-6f) {
                continue;
            }

            float v = (d11 * d20 - d01 * d21) / denom;
            float w = (d00 * d21 - d01 * d20) / denom;
            float u = 1.0f - v - w;

            if (u >= 0.0f && v >= 0.0f && w >= 0.0f && (u + v + w) <= 1.0f) {
                drawPoint(x, y, color);
            }
        }
    }
}

bool isInsidePlane(const Vec4& position, int planeIndex, int sign) {
    switch (planeIndex) {
        case 0:
            return sign * position.x <= position.w;
        case 1:
            return sign * position.y <= position.w;
        case 2:
            return position.z >= -position.w;
        case 3:
            return position.z <= position.w;
        default:
            return false;
    }
}

float intersectionParameter(const Vec4& v1, const Vec4& v2, int planeIndex, int sign) {
    float t = 0.0f;
    
    switch (planeIndex) {
        case 0:
            t = (sign * v1.w - v1.x) / ((v2.x - v1.x) - sign * (v2.w - v1.w));
            break;
        case 1:
            t = (sign * v1.w - v1.y) / ((v2.y - v1.y) - sign * (v2.w - v1.w));
            break;
        case 2:
            t = (v1.z + v1.w) / ((v1.w - v2.w) - (v1.z - v2.z));
            break;
        case 3:
            t = (v1.w - v1.z) / ((v1.z - v2.z) + (v1.w - v2.w));
            break;
    }
    
    return std::max(0.0f, std::min(1.0f, t));
}

// Sutherland-Hodgman Polygon Clipping with attribute interpolation
std::vector<VertexWithAttributes> clipAgainstPlaneWithAttributes(
    const std::vector<VertexWithAttributes>& vertices, int planeIndex, int sign) {
    
    if (vertices.empty()) {
        return {};
    }
    
    std::vector<VertexWithAttributes> outputVertices;
    
    const VertexWithAttributes* previous = &vertices.back();
    
    for (const auto& current : vertices) {
        bool previousInside = isInsidePlane(previous->position, planeIndex, sign);
        bool currentInside = isInsidePlane(current.position, planeIndex, sign);

        if (previousInside && currentInside) {
            outputVertices.push_back(current);
        }
        else if (!previousInside && currentInside) {
            float t = intersectionParameter(previous->position, current.position, planeIndex, sign);
            VertexWithAttributes intersection;
            intersection.position = previous->position + (current.position - previous->position) * t;
            intersection.attributes = VertexShaderOutput::interpolate(previous->attributes, current.attributes, t);
            
            outputVertices.push_back(intersection);
            outputVertices.push_back(current);
        }
        else if (previousInside && !currentInside) {
            float t = intersectionParameter(previous->position, current.position, planeIndex, sign);
            VertexWithAttributes intersection;
            intersection.position = previous->position + (current.position - previous->position) * t;
            intersection.attributes = VertexShaderOutput::interpolate(previous->attributes, current.attributes, t);
            
            outputVertices.push_back(intersection);
        }
        
        previous = &current;
    }
    
    return outputVertices;
}

std::vector<VertexWithAttributes> clipTriangleWithAttributes(
    const VertexWithAttributes& v1, 
    const VertexWithAttributes& v2, 
    const VertexWithAttributes& v3) {
    
    std::vector<VertexWithAttributes> vertices = {v1, v2, v3};
    
    vertices = clipAgainstPlaneWithAttributes(vertices, 0, 1);
    vertices = clipAgainstPlaneWithAttributes(vertices, 0, -1);
    vertices = clipAgainstPlaneWithAttributes(vertices, 1, 1);
    vertices = clipAgainstPlaneWithAttributes(vertices, 1, -1);
    vertices = clipAgainstPlaneWithAttributes(vertices, 2, 1);
    vertices = clipAgainstPlaneWithAttributes(vertices, 3, 0);
    
    return vertices;
}

void Rasterizer::renderMesh(const Mesh& mesh, const Shader& shader) {
    const std::vector<Vertex>& vertices = mesh.getVertices();
    const std::vector<Triangle>& triangles = mesh.getTriangles();
    Matrix4x4 modelMatrix = mesh.getModelMatrix();
    
    LOG_DEBUG("Rendering mesh with " + std::to_string(vertices.size()) + " vertices and " + 
             std::to_string(triangles.size()) + " triangles");

    for (const Triangle& triangle : triangles) {
        const Vertex& v1 = vertices[triangle.v1];
        const Vertex& v2 = vertices[triangle.v2];
        const Vertex& v3 = vertices[triangle.v3];

        VertexShaderInput in1{v1.position, v1.normal, v1.texCoord, v1.color};
        VertexShaderInput in2{v2.position, v2.normal, v2.texCoord, v2.color};
        VertexShaderInput in3{v3.position, v3.normal, v3.texCoord, v3.color};

        VertexShaderOutput out1 = shader.vertexShader(in1, modelMatrix); 
        VertexShaderOutput out2 = shader.vertexShader(in2, modelMatrix);
        VertexShaderOutput out3 = shader.vertexShader(in3, modelMatrix);

        Vec3 vertexNormal1 = out1.normal.normalized();
        Vec3 vertexNormal2 = out2.normal.normalized();
        Vec3 vertexNormal3 = out3.normal.normalized();

        Vec3 triangleCenter = (out1.worldPos + out2.worldPos + out3.worldPos) / 3.0f;
        Vec3 cameraPos = shader.getCameraPosition();
        Vec3 viewDir = (cameraPos - triangleCenter).normalized();

        Vec3 edge1 = (out2.worldPos - out1.worldPos);
        Vec3 edge2 = (out3.worldPos - out1.worldPos);
        Vec3 normal = edge1.cross(edge2).normalized();

        Vec3 avgVertexNormal = (vertexNormal1 + vertexNormal2 + vertexNormal3).normalized();
        float vertexNormalDot = avgVertexNormal.dot(viewDir);
        float faceNormalDot = normal.dot(viewDir);

        float bestDotProduct = std::max(vertexNormalDot, faceNormalDot);

        if (!m_wireframeMode && bestDotProduct < -0.7f) {
            LOG_DEBUG("Triangle culled due to backface culling");
            continue;
        }

        VertexWithAttributes va1(out1.position, out1);
        VertexWithAttributes va2(out2.position, out2);
        VertexWithAttributes va3(out3.position, out3);

        std::vector<VertexWithAttributes> clippedVertices = clipTriangleWithAttributes(va1, va2, va3);
        
        if (clippedVertices.size() < 3) {
            LOG_DEBUG("Triangle clipped out");
            continue;
        }

        LOG_DEBUG("Clipped triangle with " + std::to_string(clippedVertices.size()) + " vertices");

        std::vector<Vec4> screens;

        for (const auto& vertex : clippedVertices) {
            Vec4 ndc = vertex.position / vertex.position.w;
            Vec4 screen = viewportTransform(ndc);
            screens.push_back(screen);
        }

        int counter = 0;

        for (size_t i = 1; i < clippedVertices.size() - 1; i++) {
            const VertexWithAttributes& clipVert1 = clippedVertices[0];
            const VertexWithAttributes& clipVert2 = clippedVertices[i];
            const VertexWithAttributes& clipVert3 = clippedVertices[i + 1];

            const VertexShaderOutput& clipOut1 = clipVert1.attributes;
            const VertexShaderOutput& clipOut2 = clipVert2.attributes;
            const VertexShaderOutput& clipOut3 = clipVert3.attributes;
            
            Vec4 ndc1 = clipVert1.position / clipVert1.position.w;
            Vec4 ndc2 = clipVert2.position / clipVert2.position.w;
            Vec4 ndc3 = clipVert3.position / clipVert3.position.w;

            Vec4 screen1 = screens[0];
            Vec4 screen2 = screens[i];
            Vec4 screen3 = screens[i + 1];

            int minX = std::max(0, std::min(std::min(static_cast<int>(screen1.x), static_cast<int>(screen2.x)), static_cast<int>(screen3.x)));
            int maxX = std::min(m_width - 1, std::max(std::max(static_cast<int>(screen1.x), static_cast<int>(screen2.x)), static_cast<int>(screen3.x)));
            int minY = std::max(0, std::min(std::min(static_cast<int>(screen1.y), static_cast<int>(screen2.y)), static_cast<int>(screen3.y)));
            int maxY = std::min(m_height - 1, std::max(std::max(static_cast<int>(screen1.y), static_cast<int>(screen2.y)), static_cast<int>(screen3.y)));

            for (int y = minY; y <= maxY; y++) {
                for (int x = minX; x <= maxX; x++) {
                    Vec2 p(static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f);

                    Vec2 a(screen1.x, screen1.y);
                    Vec2 b(screen2.x, screen2.y);
                    Vec2 c(screen3.x, screen3.y);

                    Vec2 v0 = b - a;
                    Vec2 v1 = c - a;
                    Vec2 v2 = p - a;

                    float d00 = v0.dot(v0);
                    float d01 = v0.dot(v1);
                    float d11 = v1.dot(v1);
                    float d20 = v2.dot(v0);
                    float d21 = v2.dot(v1);

                    float denom = d00 * d11 - d01 * d01;
                    if (std::abs(denom) < 1e-6f) {
                        continue;
                    }

                    float beta = (d11 * d20 - d01 * d21) / denom;
                    float gamma = (d00 * d21 - d01 * d20) / denom;
                    float alpha = 1.0f - beta - gamma;

                    if (alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f &&
                        (alpha + beta + gamma) <= 1.0f + 1e-5f) {

                        float w1 = 1.0f / clipVert1.position.w;
                        float w2 = 1.0f / clipVert2.position.w;
                        float w3 = 1.0f / clipVert3.position.w;

                        float wInterp = alpha * w1 + beta * w2 + gamma * w3;
                        float z1 = ndc1.z;
                        float z2 = ndc2.z;
                        float z3 = ndc3.z;

                        float zInterp = (alpha * z1 * w1 + beta * z2 * w2 + gamma * z3 * w3) / wInterp;

                        int index = y * m_width + x;
                        float facingRatio = normal.dot(viewDir);
                        float bias = 0.00001f * (1.0f - facingRatio);
                        float depthValue = zInterp - bias;
                        
                        if (depthValue < m_depthBuffer[index]) {
                            float alphaPersp = w1 * alpha / wInterp;
                            float betaPersp = w2 * beta / wInterp;
                            float gammaPersp = w3 * gamma / wInterp;
                            
                            Vec3 worldPos = clipOut1.worldPos * alphaPersp + clipOut2.worldPos * betaPersp + clipOut3.worldPos * gammaPersp;
                            Vec3 normal = (clipOut1.normal * alphaPersp + clipOut2.normal * betaPersp + clipOut3.normal * gammaPersp).normalized();
                            
                            Vec2 texCoord = Vec2(
                                clipOut1.texCoord.x * alphaPersp + clipOut2.texCoord.x * betaPersp + clipOut3.texCoord.x * gammaPersp,
                                clipOut1.texCoord.y * alphaPersp + clipOut2.texCoord.y * betaPersp + clipOut3.texCoord.y * gammaPersp
                            );
                            
                            Color baseColor = Color(
                                static_cast<uint8_t>(clipOut1.color.r * alphaPersp + clipOut2.color.r * betaPersp + clipOut3.color.r * gammaPersp),
                                static_cast<uint8_t>(clipOut1.color.g * alphaPersp + clipOut2.color.g * betaPersp + clipOut3.color.g * gammaPersp),
                                static_cast<uint8_t>(clipOut1.color.b * alphaPersp + clipOut2.color.b * betaPersp + clipOut3.color.b * gammaPersp),
                                static_cast<uint8_t>(clipOut1.color.a * alphaPersp + clipOut2.color.a * betaPersp + clipOut3.color.a * gammaPersp)
                            );
                            
                            float shadowFactor = getShadowFactor(worldPos);
                            
                            Vec4 shadowPos = clipOut1.shadowPos * alphaPersp + clipOut2.shadowPos * betaPersp + clipOut3.shadowPos * gammaPersp;

                            FragmentShaderInput fragIn{worldPos, normal, texCoord, baseColor, shadowPos, shadowFactor};
                            Color pixelColor = shader.fragmentShader(fragIn);

                            m_colorBuffer[index] = pixelColor.toUint32();
                            m_depthBuffer[index] = depthValue;

                            counter++;
                        }
                    }
                }
            }

            if (m_wireframeMode) {
                Color wireColor = normal.dot(viewDir) > 0.0f
                    ? Color(255, 255, 255)
                    : Color(255, 0, 0);
                drawLine(
                    static_cast<int>(screen1.x), static_cast<int>(screen1.y),
                    static_cast<int>(screen2.x), static_cast<int>(screen2.y),
                    wireColor
                );
                drawLine(
                    static_cast<int>(screen2.x), static_cast<int>(screen2.y),
                    static_cast<int>(screen3.x), static_cast<int>(screen3.y),
                    wireColor
                );
                drawLine(
                    static_cast<int>(screen3.x), static_cast<int>(screen3.y),
                    static_cast<int>(screen1.x), static_cast<int>(screen1.y),
                    wireColor
                );
            }
        }
        LOG_DEBUG("Rendered " + std::to_string(counter) + " pixels");
    }
}

void Rasterizer::beginShadowPass() {
    std::fill(m_shadowMap.begin(), m_shadowMap.end(), 1.0f);
    m_shadowsEnabled = true;
}

float Rasterizer::getShadowFactor(const Vec3& worldPos) const {
    if (!m_shadowsEnabled) {
        return 1.0f;
    }

    Vec4 shadowPos = m_shadowMatrix * Vec4(worldPos.x, worldPos.y, worldPos.z, 1.0f);
    
    if (std::abs(shadowPos.w) < 0.0001f) {
        return 1.0f;
    }
    
    shadowPos = shadowPos / shadowPos.w;
    
    float shadowX = (shadowPos.x + 1.0f) * 0.5f;
    float shadowY = (1.0f - shadowPos.y) * 0.5f;
    float shadowDepth = (shadowPos.z + 1.0f) * 0.5f;
    
    if (shadowX < 0.0f || shadowX > 1.0f || shadowY < 0.0f || shadowY > 1.0f || shadowDepth > 1.0f) {
        return 1.0f;
    }
    
    int mapX = static_cast<int>(shadowX * (SHADOW_MAP_SIZE - 1));
    int mapY = static_cast<int>(shadowY * (SHADOW_MAP_SIZE - 1));
    int shadowIndex = mapY * SHADOW_MAP_SIZE + mapX;
    
    if (shadowIndex < 0 || shadowIndex >= m_shadowMap.size()) {
        return 1.0f;
    }
    
    float storedDepth = m_shadowMap[shadowIndex];
    
    float bias = 0.01f;
    
    float shadowFactor = 1.0f;
    
    int pcfSize = 3;
    int shadowCount = 0;
    int totalSamples = 0;
    
    for (int y = -pcfSize; y <= pcfSize; y++) {
        for (int x = -pcfSize; x <= pcfSize; x++) {
            int sampleX = mapX + x;
            int sampleY = mapY + y;
            
            if (sampleX >= 0 && sampleX < SHADOW_MAP_SIZE && 
                sampleY >= 0 && sampleY < SHADOW_MAP_SIZE) {
                
                int sampleIndex = sampleY * SHADOW_MAP_SIZE + sampleX;
                float sampleDepth = m_shadowMap[sampleIndex];
                
                if (shadowDepth - bias > sampleDepth) {
                    shadowCount++;
                }
                totalSamples++;
            }
        }
    }
    
    if (totalSamples > 0) {
        shadowFactor = 1.0f - (static_cast<float>(shadowCount) / totalSamples) * 0.85f;
    }
    
    if (shadowCount > 0 && shadowFactor > 0.5f) {
        shadowFactor = 0.5f;
    }
    
    return shadowFactor;
}

void Rasterizer::renderShadowMap(const Mesh& mesh, const Shader& shader, const Vec3& lightPos, const Vec3& lightDir) {
    if (!m_shadowsEnabled) {
        return;
    }
    
    Vec3 lightTarget = Vec3(0, 0, 0);
    Vec3 lightUp = Vec3(0, 1, 0);
    
    Vec3 lightForward = (lightTarget - lightPos).normalized();
    if (lightDir.length() > 0.001f) {
        lightForward = -lightDir.normalized();
    }
    
    if (std::abs(lightForward.dot(Vec3(0, 1, 0))) > 0.99f) {
        lightUp = Vec3(0, 0, 1);
    }
    
    Vec3 lightRight = lightForward.cross(lightUp).normalized();
    lightUp = lightRight.cross(lightForward).normalized();
    
    m_lightViewMatrix = Matrix4x4::lookAt(lightPos, lightPos + lightForward, lightUp);
    
    float shadowOrthoSize = 3.0f;
    m_lightProjectionMatrix = Matrix4x4::identity();
    
    m_lightProjectionMatrix(0, 0) = 1.0f / shadowOrthoSize;
    m_lightProjectionMatrix(1, 1) = 1.0f / shadowOrthoSize;
    m_lightProjectionMatrix(2, 2) = 2.0f / (10.0f - 0.1f);
    m_lightProjectionMatrix(2, 3) = -(10.0f + 0.1f) / (10.0f - 0.1f);
    
    m_shadowMatrix = m_lightProjectionMatrix * m_lightViewMatrix;
    
    const std::vector<Vertex>& vertices = mesh.getVertices();
    const std::vector<Triangle>& triangles = mesh.getTriangles();
    
    for (const Triangle& triangle : triangles) {
        const Vertex& v1 = vertices[triangle.v1];
        const Vertex& v2 = vertices[triangle.v2];
        const Vertex& v3 = vertices[triangle.v3];
        
        Vec4 worldPos1 = mesh.getModelMatrix() * Vec4(v1.position.x, v1.position.y, v1.position.z, 1.0f);
        Vec4 worldPos2 = mesh.getModelMatrix() * Vec4(v2.position.x, v2.position.y, v2.position.z, 1.0f);
        Vec4 worldPos3 = mesh.getModelMatrix() * Vec4(v3.position.x, v3.position.y, v3.position.z, 1.0f);
        
        Vec4 lightSpacePos1 = m_lightProjectionMatrix * m_lightViewMatrix * worldPos1;
        Vec4 lightSpacePos2 = m_lightProjectionMatrix * m_lightViewMatrix * worldPos2;
        Vec4 lightSpacePos3 = m_lightProjectionMatrix * m_lightViewMatrix * worldPos3;
        
        Vec4 ndcPos1 = lightSpacePos1 / lightSpacePos1.w;
        Vec4 ndcPos2 = lightSpacePos2 / lightSpacePos2.w;
        Vec4 ndcPos3 = lightSpacePos3 / lightSpacePos3.w;
        
        Vec4 shadowPos1 = Vec4((ndcPos1.x + 1.0f) * 0.5f, (1.0f - ndcPos1.y) * 0.5f, (ndcPos1.z + 1.0f) * 0.5f, 1.0f);
        Vec4 shadowPos2 = Vec4((ndcPos2.x + 1.0f) * 0.5f, (1.0f - ndcPos2.y) * 0.5f, (ndcPos2.z + 1.0f) * 0.5f, 1.0f);
        Vec4 shadowPos3 = Vec4((ndcPos3.x + 1.0f) * 0.5f, (1.0f - ndcPos3.y) * 0.5f, (ndcPos3.z + 1.0f) * 0.5f, 1.0f);
        
        Vec4 pixelPos1 = shadowPos1 * static_cast<float>(SHADOW_MAP_SIZE);
        Vec4 pixelPos2 = shadowPos2 * static_cast<float>(SHADOW_MAP_SIZE);
        Vec4 pixelPos3 = shadowPos3 * static_cast<float>(SHADOW_MAP_SIZE);
        
        int minX = std::max(0, std::min(std::min(static_cast<int>(pixelPos1.x), static_cast<int>(pixelPos2.x)), static_cast<int>(pixelPos3.x)));
        int maxX = std::min(SHADOW_MAP_SIZE - 1, std::max(std::max(static_cast<int>(pixelPos1.x), static_cast<int>(pixelPos2.x)), static_cast<int>(pixelPos3.x)));
        int minY = std::max(0, std::min(std::min(static_cast<int>(pixelPos1.y), static_cast<int>(pixelPos2.y)), static_cast<int>(pixelPos3.y)));
        int maxY = std::min(SHADOW_MAP_SIZE - 1, std::max(std::max(static_cast<int>(pixelPos1.y), static_cast<int>(pixelPos2.y)), static_cast<int>(pixelPos3.y)));
        
        for (int y = minY; y <= maxY; y++) {
            for (int x = minX; x <= maxX; x++) {
                Vec2 p(static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f);
                
                Vec2 v0(pixelPos1.x, pixelPos1.y);
                Vec2 v1(pixelPos2.x, pixelPos2.y);
                Vec2 v2(pixelPos3.x, pixelPos3.y);
                
                Vec2 e0 = v1 - v0;
                Vec2 e1 = v2 - v0;
                Vec2 e2 = p - v0;
                
                float d00 = e0.dot(e0);
                float d01 = e0.dot(e1);
                float d11 = e1.dot(e1);
                float d20 = e2.dot(e0);
                float d21 = e2.dot(e1);
                
                float denom = d00 * d11 - d01 * d01;
                if (std::abs(denom) < 1e-6f) {
                    continue;
                }
                
                float beta = (d11 * d20 - d01 * d21) / denom;
                float gamma = (d00 * d21 - d01 * d20) / denom;
                float alpha = 1.0f - beta - gamma;
                
                if (alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f && (alpha + beta + gamma) <= 1.0f + 1e-5f) {
                    float depth = alpha * shadowPos1.z + beta * shadowPos2.z + gamma * shadowPos3.z;
                    
                    int index = y * SHADOW_MAP_SIZE + x;
                    if (depth < m_shadowMap[index]) {
                        m_shadowMap[index] = depth;
                    }
                }
            }
        }
    }
}

void Rasterizer::present() {
    SDL_UpdateTexture(m_frameBuffer, nullptr, m_colorBuffer.data(), m_width * sizeof(uint32_t));

    SDL_RenderClear(m_renderer);

    SDL_RenderCopy(m_renderer, m_frameBuffer, nullptr, nullptr);

    SDL_RenderPresent(m_renderer);
}

bool Rasterizer::shouldQuit() const {
    return m_quit;
}

void Rasterizer::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            m_quit = true;

        else if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_ESCAPE)
                m_quit = true;

            else if (event.key.keysym.sym == SDLK_w)
            {
                m_wireframeMode = !m_wireframeMode;
                LOG_INFO("Wireframe mode: " + std::string(m_wireframeMode ? "ON" : "OFF"));
            }

            else if (event.key.keysym.sym == SDLK_d)
            {
                LogLevel currentLevel = Logger::getInstance().getLevel();
                if (currentLevel == LogLevel::INFO)
                {
                    Logger::getInstance().setLevel(LogLevel::DEBUG);
                    LOG_INFO("Debug logging enabled");
                }
                else
                {
                    Logger::getInstance().setLevel(LogLevel::INFO);
                    LOG_INFO("Debug logging disabled");
                }
            }
        }
    }
}

Vec4 Rasterizer::viewportTransform(const Vec4& clipCoords) const {
    float x = (clipCoords.x + 1.0f) * 0.5f * m_width;
    float y = (1.0f - clipCoords.y) * 0.5f * m_height;

    float z = (clipCoords.z + 1.0f) * 0.5f;

    z = std::max(0.0001f, std::min(0.9999f, z));

    return Vec4(x, y, z, clipCoords.w);
}

bool Rasterizer::isInsideFrustum(const Vec4& clipCoords) const {
    float w = std::abs(clipCoords.w);

    float margin = 0.1f * w;

    return clipCoords.x >= -(w + margin) && clipCoords.x <= (w + margin) &&
           clipCoords.y >= -(w + margin) && clipCoords.y <= (w + margin) &&
           clipCoords.z >= -w && clipCoords.z <= w;
}
