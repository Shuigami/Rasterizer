#pragma once

#include "vector.h"
#include "matrix.h"
#include "texture.h"
#include <memory>

struct VertexShaderInput {
    Vec3 position;
    Vec3 normal;
    Vec2 texCoord;
    Color color;
};

struct VertexShaderOutput {
    Vec4 position;
    Vec3 worldPos;
    Vec3 normal;
    Vec2 texCoord;
    Color color;

    // Interpolation between two vertex outputs
    static VertexShaderOutput interpolate(const VertexShaderOutput& v1, const VertexShaderOutput& v2, float t) {
        VertexShaderOutput result;
        result.position = v1.position * (1.0f - t) + v2.position * t;
        result.worldPos = v1.worldPos * (1.0f - t) + v2.worldPos * t;
        result.normal = v1.normal * (1.0f - t) + v2.normal * t;
        result.texCoord = Vec2(
            v1.texCoord.x * (1.0f - t) + v2.texCoord.x * t,
            v1.texCoord.y * (1.0f - t) + v2.texCoord.y * t
        );

        // Interpolate colors
        float r = v1.color.r * (1.0f - t) + v2.color.r * t;
        float g = v1.color.g * (1.0f - t) + v2.color.g * t;
        float b = v1.color.b * (1.0f - t) + v2.color.b * t;
        float a = v1.color.a * (1.0f - t) + v2.color.a * t;

        result.color = Color(
            static_cast<uint8_t>(r),
            static_cast<uint8_t>(g),
            static_cast<uint8_t>(b),
            static_cast<uint8_t>(a)
        );

        return result;
    }

    // Barycentric interpolation for triangles
    static VertexShaderOutput barycentricInterpolate(
        const VertexShaderOutput& v1,
        const VertexShaderOutput& v2,
        const VertexShaderOutput& v3,
        float w1, float w2, float w3) {

        VertexShaderOutput result;
        result.position = v1.position * w1 + v2.position * w2 + v3.position * w3;
        result.worldPos = v1.worldPos * w1 + v2.worldPos * w2 + v3.worldPos * w3;
        result.normal = v1.normal * w1 + v2.normal * w2 + v3.normal * w3;
        result.texCoord = Vec2(
            v1.texCoord.x * w1 + v2.texCoord.x * w2 + v3.texCoord.x * w3,
            v1.texCoord.y * w1 + v2.texCoord.y * w2 + v3.texCoord.y * w3
        );

        // Interpolate colors
        float r = v1.color.r * w1 + v2.color.r * w2 + v3.color.r * w3;
        float g = v1.color.g * w1 + v2.color.g * w2 + v3.color.g * w3;
        float b = v1.color.b * w1 + v2.color.b * w2 + v3.color.b * w3;
        float a = v1.color.a * w1 + v2.color.a * w2 + v3.color.a * w3;

        result.color = Color(
            static_cast<uint8_t>(r),
            static_cast<uint8_t>(g),
            static_cast<uint8_t>(b),
            static_cast<uint8_t>(a)
        );

        return result;
    }
};

struct FragmentShaderInput {
    Vec3 worldPos;
    Vec3 normal;
    Vec2 texCoord;
    Color color;
};

struct Light {
    enum class Type {
        Directional,
        Point,
        Spot
    };

    Type type;
    Vec3 position;    // Used for point and spot lights
    Vec3 direction;   // Used for directional and spot lights
    Color color;
    float intensity;
    float range;      // Used for point and spot lights
    float spotAngle;  // Used for spot lights (in radians)

    Light()
        : type(Type::Directional),
          position(0.0f, 0.0f, 0.0f),
          direction(0.0f, -1.0f, 0.0f),
          color(255, 255, 255),
          intensity(1.0f),
          range(10.0f),
          spotAngle(0.5f) {}
};

class Shader {
public:
    Shader();
    virtual ~Shader();

    void setModelMatrix(const Matrix4x4& model) { m_model = model; }
    void setViewMatrix(const Matrix4x4& view) { m_view = view; }
    void setProjectionMatrix(const Matrix4x4& projection) { m_projection = projection; }
    void setTexture(const std::shared_ptr<Texture>& texture) { m_texture = texture; }

    // Add a light to the scene
    void addLight(const Light& light) {
        m_lights.push_back(light);
    }

    // Clear all lights
    void clearLights() {
        m_lights.clear();
    }

    // Vertex shader - transforms vertices
    virtual VertexShaderOutput vertexShader(const VertexShaderInput& input) const;

    // Fragment shader - computes pixel color
    virtual Color fragmentShader(const FragmentShaderInput& input) const;

protected:
    Matrix4x4 m_model;
    Matrix4x4 m_view;
    Matrix4x4 m_projection;
    std::shared_ptr<Texture> m_texture;
    std::vector<Light> m_lights;
};

// Flat color shader
class FlatShader : public Shader {
public:
    FlatShader(const Color& color);
    Color fragmentShader(const FragmentShaderInput& input) const override;

private:
    Color m_color;
};

// Texture shader
class TextureShader : public Shader {
public:
    TextureShader();
    Color fragmentShader(const FragmentShaderInput& input) const override;
};

// Phong lighting shader
class PhongShader : public Shader {
public:
    PhongShader();

    void setAmbient(float ambient) { m_ambient = ambient; }
    void setDiffuse(float diffuse) { m_diffuse = diffuse; }
    void setSpecular(float specular) { m_specular = specular; }
    void setShininess(float shininess) { m_shininess = shininess; }
    void setCameraPosition(const Vec3& cameraPos) { m_cameraPos = cameraPos; }

    Color fragmentShader(const FragmentShaderInput& input) const override;

private:
    float m_ambient;
    float m_diffuse;
    float m_specular;
    float m_shininess;
    Vec3 m_cameraPos;
};
