#include "rasterizer.h"
#include <algorithm>
#include <iostream>
#include <limits>

Rasterizer::Rasterizer(int width, int height)
    : m_width(width), m_height(height), m_window(nullptr), m_renderer(nullptr),
      m_frameBuffer(nullptr), m_quit(false) {

    // Initialize buffers
    m_colorBuffer.resize(width * height, 0);
    m_depthBuffer.resize(width * height, 1.0f); // Initialize to 1.0f (furthest possible depth)
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
    // Create window
    m_window = SDL_CreateWindow(
        "Software Rasterizer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        m_width, m_height,
        SDL_WINDOW_SHOWN
    );

    if (!m_window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (!m_renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create frame buffer texture
    m_frameBuffer = SDL_CreateTexture(
        m_renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        m_width, m_height
    );

    if (!m_frameBuffer) {
        std::cerr << "Failed to create frame buffer: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void Rasterizer::clear(const Color& color) {
    uint32_t clearColor = color.toUint32();
    std::fill(m_colorBuffer.begin(), m_colorBuffer.end(), clearColor);
    
    // Reset depth buffer to maximum value (1.0)
    // Use std::numeric_limits for maximum precision
    std::fill(m_depthBuffer.begin(), m_depthBuffer.end(), std::numeric_limits<float>::max());
}

void Rasterizer::drawPoint(int x, int y, const Color& color) {
    // Check if the point is within the viewport
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return;
    }

    // Draw the pixel
    m_colorBuffer[y * m_width + x] = color.toUint32();
}

void Rasterizer::drawLine(int x1, int y1, int x2, int y2, const Color& color) {
    // Bresenham's line algorithm
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
    // Draw the three sides of the triangle
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
    // Calculate bounding box
    int minX = std::max(0, std::min(std::min(static_cast<int>(v1.x), static_cast<int>(v2.x)), static_cast<int>(v3.x)));
    int maxX = std::min(m_width - 1, std::max(std::max(static_cast<int>(v1.x), static_cast<int>(v2.x)), static_cast<int>(v3.x)));
    int minY = std::max(0, std::min(std::min(static_cast<int>(v1.y), static_cast<int>(v2.y)), static_cast<int>(v3.y)));
    int maxY = std::min(m_height - 1, std::max(std::max(static_cast<int>(v1.y), static_cast<int>(v2.y)), static_cast<int>(v3.y)));

    // Iterate over each pixel in the bounding box
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            // Barycentric coordinates
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

            // Check if the point is inside the triangle
            if (u >= 0.0f && v >= 0.0f && w >= 0.0f && (u + v + w) <= 1.0f) {
                drawPoint(x, y, color);
            }
        }
    }
}

void Rasterizer::renderMesh(const Mesh& mesh, const Matrix4x4& modelMatrix, const Shader& shader, bool wireframeMode) {
    const std::vector<Vertex>& vertices = mesh.getVertices();
    const std::vector<Triangle>& triangles = mesh.getTriangles();

    // Process each triangle
    for (const Triangle& triangle : triangles) {
        // Get the vertices of the triangle
        const Vertex& v1 = vertices[triangle.v1];
        const Vertex& v2 = vertices[triangle.v2];
        const Vertex& v3 = vertices[triangle.v3];

        // Create vertex shader inputs
        VertexShaderInput in1{v1.position, v1.normal, v1.texCoord, v1.color};
        VertexShaderInput in2{v2.position, v2.normal, v2.texCoord, v2.color};
        VertexShaderInput in3{v3.position, v3.normal, v3.texCoord, v3.color};

        // Process vertices through the vertex shader
        VertexShaderOutput out1 = shader.vertexShader(in1);
        VertexShaderOutput out2 = shader.vertexShader(in2);
        VertexShaderOutput out3 = shader.vertexShader(in3);

        // Check if the triangle is facing the camera (backface culling)
        Vec3 edge1 = (out2.worldPos - out1.worldPos);
        Vec3 edge2 = (out3.worldPos - out1.worldPos);
        Vec3 normal = edge1.cross(edge2).normalized();
        
        // Calculate the center of the triangle
        Vec3 triangleCenter = (out1.worldPos + out2.worldPos + out3.worldPos) / 3.0f;
        
        // Get camera position for backface culling
        Vec3 cameraPos = shader.getCameraPosition();
        
        // Use the camera-to-face vector for culling, based on triangle center
        Vec3 viewDir = (cameraPos - triangleCenter).normalized();
        
        // Skip backface culling in wireframe mode
        // Use a slightly negative threshold to prevent z-fighting on coplanar faces
        if (!wireframeMode && normal.dot(viewDir) < -0.001f) {
            continue; // Skip triangles facing away from the camera
        }

        // Check if any of the vertices are outside the frustum
        if (!isInsideFrustum(out1.position) && !isInsideFrustum(out2.position) && !isInsideFrustum(out3.position)) {
            continue;
        }

        // Perspective divide to get normalized device coordinates
        Vec4 ndc1 = out1.position / out1.position.w;
        Vec4 ndc2 = out2.position / out2.position.w;
        Vec4 ndc3 = out3.position / out3.position.w;

        // Viewport transform
        Vec4 screen1 = viewportTransform(ndc1);
        Vec4 screen2 = viewportTransform(ndc2);
        Vec4 screen3 = viewportTransform(ndc3);

        // Ensure the triangle is visible in the viewport before rasterizing
        if (screen1.x < 0 && screen2.x < 0 && screen3.x < 0) continue;
        if (screen1.x >= m_width && screen2.x >= m_width && screen3.x >= m_width) continue;
        if (screen1.y < 0 && screen2.y < 0 && screen3.y < 0) continue;
        if (screen1.y >= m_height && screen2.y >= m_height && screen3.y >= m_height) continue;
        
        // Calculate bounding box for the triangle
        int minX = std::max(0, std::min(std::min(static_cast<int>(screen1.x), static_cast<int>(screen2.x)), static_cast<int>(screen3.x)));
        int maxX = std::min(m_width - 1, std::max(std::max(static_cast<int>(screen1.x), static_cast<int>(screen2.x)), static_cast<int>(screen3.x)));
        int minY = std::max(0, std::min(std::min(static_cast<int>(screen1.y), static_cast<int>(screen2.y)), static_cast<int>(screen3.y)));
        int maxY = std::min(m_height - 1, std::max(std::max(static_cast<int>(screen1.y), static_cast<int>(screen2.y)), static_cast<int>(screen3.y)));

        // Iterate over each pixel in the bounding box
        for (int y = minY; y <= maxY; y++) {
            for (int x = minX; x <= maxX; x++) {
                // Barycentric coordinates
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

                // Check if the point is inside the triangle
                if (alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f &&
                    (alpha + beta + gamma) <= 1.0f + 1e-5f) {

                    // Perspective correct interpolation weights
                    float w1 = 1.0f / out1.position.w;
                    float w2 = 1.0f / out2.position.w;
                    float w3 = 1.0f / out3.position.w;

                    float wInterp = alpha * w1 + beta * w2 + gamma * w3;
                    float zInterp = alpha * screen1.z * w1 + beta * screen2.z * w2 + gamma * screen3.z * w3;

                    // Depth test
                    int index = y * m_width + x;
                    // Apply a small bias based on how front-facing the triangle is
                    float facingRatio = normal.dot(viewDir);
                    float bias = 0.00001f * (1.0f - facingRatio);
                    float depthValue = zInterp - bias;
                    if (depthValue < m_depthBuffer[index]) {
                        // Perspective-correct interpolation
                        alpha *= w1 / wInterp;
                        beta *= w2 / wInterp;
                        gamma *= w3 / wInterp;

                        // Interpolate vertex data
                        Vec3 worldPos = out1.worldPos * alpha + out2.worldPos * beta + out3.worldPos * gamma;
                        Vec3 normal = (out1.normal * alpha + out2.normal * beta + out3.normal * gamma).normalized();
                        Vec2 texCoord = Vec2(
                            out1.texCoord.x * alpha + out2.texCoord.x * beta + out3.texCoord.x * gamma,
                            out1.texCoord.y * alpha + out2.texCoord.y * beta + out3.texCoord.y * gamma
                        );
                        Color baseColor = Color(
                            static_cast<uint8_t>(out1.color.r * alpha + out2.color.r * beta + out3.color.r * gamma),
                            static_cast<uint8_t>(out1.color.g * alpha + out2.color.g * beta + out3.color.g * gamma),
                            static_cast<uint8_t>(out1.color.b * alpha + out2.color.b * beta + out3.color.b * gamma),
                            static_cast<uint8_t>(out1.color.a * alpha + out2.color.a * beta + out3.color.a * gamma)
                        );

                        // Normalize interpolated normal to ensure proper lighting
                        Vec3 normalizedNormal = normal.normalized();
                        
                        // Create fragment input
                        FragmentShaderInput fragIn{worldPos, normalizedNormal, texCoord, baseColor};

                        // Process fragment through the fragment shader
                        Color pixelColor = shader.fragmentShader(fragIn);

                        // Update buffers
                        m_colorBuffer[index] = pixelColor.toUint32();
                        m_depthBuffer[index] = depthValue;
                    }
                }
            }
            
            // Draw wireframe if enabled
            if (wireframeMode) {
                // Use different colors based on whether the face is front or back facing
                Color wireColor = normal.dot(viewDir) > 0.0f 
                    ? Color(255, 255, 255)  // White for front facing
                    : Color(255, 0, 0);     // Red for back facing
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
    // Update texture with the color buffer
    SDL_UpdateTexture(m_frameBuffer, nullptr, m_colorBuffer.data(), m_width * sizeof(uint32_t));

    // Clear renderer
    SDL_RenderClear(m_renderer);

    // Render the frame buffer
    SDL_RenderCopy(m_renderer, m_frameBuffer, nullptr, nullptr);

    // Present renderer
    SDL_RenderPresent(m_renderer);
}

bool Rasterizer::shouldQuit() const {
    return m_quit;
}

void Rasterizer::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            m_quit = true;
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                m_quit = true;
            }
        }
    }
}

Vec4 Rasterizer::viewportTransform(const Vec4& clipCoords) const {
    float x = (clipCoords.x + 1.0f) * 0.5f * m_width;
    float y = (1.0f - clipCoords.y) * 0.5f * m_height; // Flip y-coordinate
    float z = clipCoords.z; // Depth value in [0, 1]
    
    // Ensure z is within valid range [0, 1]
    z = std::max(0.0f, std::min(1.0f, z));

    return Vec4(x, y, z, clipCoords.w);
}

bool Rasterizer::isInsideFrustum(const Vec4& clipCoords) const {
    // Check if the point is inside the view frustum in clip space
    float w = std::abs(clipCoords.w);
    
    // We need at least one vertex to be inside the frustum
    return clipCoords.x >= -w && clipCoords.x <= w &&
           clipCoords.y >= -w && clipCoords.y <= w &&
           clipCoords.z >= 0.0f && clipCoords.z <= w; // Only render in front of the camera
}
