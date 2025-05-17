#include "rasterizer.h"
#include "logger.h"
#include <algorithm>
#include <iostream>

// Define a struct to track vertex positions and attributes during clipping
struct VertexWithAttributes {
    Vec4 position;
    VertexShaderOutput attributes;
    
    VertexWithAttributes() {}
    
    VertexWithAttributes(const Vec4& pos, const VertexShaderOutput& attr) 
        : position(pos), attributes(attr) {}
};

Rasterizer::Rasterizer(int width, int height)
    : m_width(width), m_height(height), m_window(nullptr), m_renderer(nullptr),
      m_frameBuffer(nullptr), m_quit(false) {

    m_colorBuffer.resize(width * height, 0);
    m_depthBuffer.resize(width * height, 1.0f);
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

        LOG_DEBUG("Clipping against plane " + std::to_string(planeIndex) + 
            " with sign " + std::to_string(sign) + ": " +
            "Previous inside: " + std::to_string(previousInside) + ", " +
            "Current inside: " + std::to_string(currentInside));

        LOG_DEBUG("Previous vertex: " + std::to_string(previous->position.x) + ", " +
            std::to_string(previous->position.y) + ", " + std::to_string(previous->position.z));
        LOG_DEBUG("Current vertex: " + std::to_string(current.position.x) + ", " +
            std::to_string(current.position.y) + ", " + std::to_string(current.position.z));
        
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
    
    LOG_DEBUG("Vertices before clipping: " +
        std::to_string(v1.position.x) + ", " + std::to_string(v1.position.y) + ", " + std::to_string(v1.position.z) + " | " +
        std::to_string(v2.position.x) + ", " + std::to_string(v2.position.y) + ", " + std::to_string(v2.position.z) + " | " +
        std::to_string(v3.position.x) + ", " + std::to_string(v3.position.y) + ", " + std::to_string(v3.position.z));
    
    LOG_DEBUG("Clipping triangle against planes");
    LOG_DEBUG("Plane: x = w");
    vertices = clipAgainstPlaneWithAttributes(vertices, 0, 1);
    for (size_t i = 0; i < vertices.size(); i++) {
        LOG_DEBUG("Vertex " + std::to_string(i) + ": " +
            std::to_string(vertices[i].position.x) + ", " +
            std::to_string(vertices[i].position.y) + ", " +
            std::to_string(vertices[i].position.z));
    }
    LOG_DEBUG("Plane: x = -w");
    vertices = clipAgainstPlaneWithAttributes(vertices, 0, -1);
    for (size_t i = 0; i < vertices.size(); i++) {
        LOG_DEBUG("Vertex " + std::to_string(i) + ": " +
            std::to_string(vertices[i].position.x) + ", " +
            std::to_string(vertices[i].position.y) + ", " +
            std::to_string(vertices[i].position.z));
    }
    LOG_DEBUG("Plane: y = w");
    vertices = clipAgainstPlaneWithAttributes(vertices, 1, 1);
    for (size_t i = 0; i < vertices.size(); i++) {
        LOG_DEBUG("Vertex " + std::to_string(i) + ": " +
            std::to_string(vertices[i].position.x) + ", " +
            std::to_string(vertices[i].position.y) + ", " +
            std::to_string(vertices[i].position.z));
    }
    LOG_DEBUG("Plane: y = -w");
    vertices = clipAgainstPlaneWithAttributes(vertices, 1, -1);
    for (size_t i = 0; i < vertices.size(); i++) {
        LOG_DEBUG("Vertex " + std::to_string(i) + ": " +
            std::to_string(vertices[i].position.x) + ", " +
            std::to_string(vertices[i].position.y) + ", " +
            std::to_string(vertices[i].position.z));
    }
    LOG_DEBUG("Plane: z = w");
    vertices = clipAgainstPlaneWithAttributes(vertices, 2, 1);
    for (size_t i = 0; i < vertices.size(); i++) {
        LOG_DEBUG("Vertex " + std::to_string(i) + ": " +
            std::to_string(vertices[i].position.x) + ", " +
            std::to_string(vertices[i].position.y) + ", " +
            std::to_string(vertices[i].position.z));
    }
    LOG_DEBUG("Plane: z = -w");
    vertices = clipAgainstPlaneWithAttributes(vertices, 3, 0);
    for (size_t i = 0; i < vertices.size(); i++) {
        LOG_DEBUG("Vertex " + std::to_string(i) + ": " +
            std::to_string(vertices[i].position.x) + ", " +
            std::to_string(vertices[i].position.y) + ", " +
            std::to_string(vertices[i].position.z));
    }
    
    return vertices;
}

void Rasterizer::renderMesh(const Mesh& mesh, const Shader& shader) {
    const std::vector<Vertex>& vertices = mesh.getVertices();
    const std::vector<Triangle>& triangles = mesh.getTriangles();
    
    LOG_DEBUG("Rendering mesh with " + std::to_string(vertices.size()) + " vertices and " + 
             std::to_string(triangles.size()) + " triangles");

    for (const Triangle& triangle : triangles) {
        const Vertex& v1 = vertices[triangle.v1];
        const Vertex& v2 = vertices[triangle.v2];
        const Vertex& v3 = vertices[triangle.v3];

        VertexShaderInput in1{v1.position, v1.normal, v1.texCoord, v1.color};
        VertexShaderInput in2{v2.position, v2.normal, v2.texCoord, v2.color};
        VertexShaderInput in3{v3.position, v3.normal, v3.texCoord, v3.color};

        VertexShaderOutput out1 = shader.vertexShader(in1);
        VertexShaderOutput out2 = shader.vertexShader(in2);
        VertexShaderOutput out3 = shader.vertexShader(in3);

        LOG_DEBUG("Vertex 1: " + std::to_string(out1.position.x) + ", " +
            std::to_string(out1.position.y) + ", " + std::to_string(out1.position.z));
        LOG_DEBUG("Vertex 2: " + std::to_string(out2.position.x) + ", " +
            std::to_string(out2.position.y) + ", " + std::to_string(out2.position.z));
        LOG_DEBUG("Vertex 3: " + std::to_string(out3.position.x) + ", " +
            std::to_string(out3.position.y) + ", " + std::to_string(out3.position.z));

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

        LOG_DEBUG("Triangle after clipping has " + std::to_string(clippedVertices.size()) + " vertices");

        for (size_t i = 0; i < clippedVertices.size(); i++) {
            LOG_DEBUG("Clipped vertex " + std::to_string(i) + ": " +
                std::to_string(clippedVertices[i].position.x) + ", " +
                std::to_string(clippedVertices[i].position.y) + ", " +
                std::to_string(clippedVertices[i].position.z));
        }
        
        for (size_t i = 1; i < clippedVertices.size() - 1; i++) {
            const VertexWithAttributes& clipVert1 = clippedVertices[0];
            const VertexWithAttributes& clipVert2 = clippedVertices[i];
            const VertexWithAttributes& clipVert3 = clippedVertices[i + 1];

            LOG_DEBUG("Filling triangle with vertices: " +
                std::to_string(clipVert1.position.x) + ", " +
                std::to_string(clipVert1.position.y) + ", " +
                std::to_string(clipVert1.position.z) + " | " +
                std::to_string(clipVert2.position.x) + ", " +
                std::to_string(clipVert2.position.y) + ", " +
                std::to_string(clipVert2.position.z) + " | " +
                std::to_string(clipVert3.position.x) + ", " +
                std::to_string(clipVert3.position.y) + ", " +
                std::to_string(clipVert3.position.z));
            
            const VertexShaderOutput& clipOut1 = clipVert1.attributes;
            const VertexShaderOutput& clipOut2 = clipVert2.attributes;
            const VertexShaderOutput& clipOut3 = clipVert3.attributes;
            
            Vec4 ndc1 = clipVert1.position / clipVert1.position.w;
            Vec4 ndc2 = clipVert2.position / clipVert2.position.w;
            Vec4 ndc3 = clipVert3.position / clipVert3.position.w;

            Vec4 screen1 = viewportTransform(ndc1);
            Vec4 screen2 = viewportTransform(ndc2);
            Vec4 screen3 = viewportTransform(ndc3);

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
                            
                            // Use the properly interpolated colors from the clipped vertices
                            Color baseColor = Color(
                                static_cast<uint8_t>(clipOut1.color.r * alphaPersp + clipOut2.color.r * betaPersp + clipOut3.color.r * gammaPersp),
                                static_cast<uint8_t>(clipOut1.color.g * alphaPersp + clipOut2.color.g * betaPersp + clipOut3.color.g * gammaPersp),
                                static_cast<uint8_t>(clipOut1.color.b * alphaPersp + clipOut2.color.b * betaPersp + clipOut3.color.b * gammaPersp),
                                static_cast<uint8_t>(clipOut1.color.a * alphaPersp + clipOut2.color.a * betaPersp + clipOut3.color.a * gammaPersp)
                            );

                            FragmentShaderInput fragIn{worldPos, normal, texCoord, baseColor};
                            Color pixelColor = shader.fragmentShader(fragIn);

                            m_colorBuffer[index] = pixelColor.toUint32();
                            m_depthBuffer[index] = depthValue;
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
