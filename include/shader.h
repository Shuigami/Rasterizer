#pragma once

#include "vector.h"
#include "matrix.h"
#include <vector>

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

    static VertexShaderOutput lerp(const VertexShaderOutput& a, const VertexShaderOutput& b, float t) {
        return {
            a.position + (b.position - a.position) * t,
            a.worldPos + (b.worldPos - a.worldPos) * t,
            a.normal + (b.normal - a.normal) * t,
            Vec2(a.texCoord.x + (b.texCoord.x - a.texCoord.x) * t,
                 a.texCoord.y + (b.texCoord.y - a.texCoord.y) * t),
            Color(
                static_cast<uint8_t>(a.color.r + (b.color.r - a.color.r) * t),
                static_cast<uint8_t>(a.color.g + (b.color.g - a.color.g) * t),
                static_cast<uint8_t>(a.color.b + (b.color.b - a.color.b) * t),
                static_cast<uint8_t>(a.color.a + (b.color.a - a.color.a) * t)
            )
        };
    }

    static VertexShaderOutput interpolate(const VertexShaderOutput& v1, const VertexShaderOutput& v2, float t) {
        VertexShaderOutput result;
        result.position = v1.position * (1.0f - t) + v2.position * t;
        result.worldPos = v1.worldPos * (1.0f - t) + v2.worldPos * t;
        result.normal = v1.normal * (1.0f - t) + v2.normal * t;
        result.texCoord = Vec2(
            v1.texCoord.x * (1.0f - t) + v2.texCoord.x * t,
            v1.texCoord.y * (1.0f - t) + v2.texCoord.y * t
        );

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
    Vec3 position;
    Vec3 direction;
    Color color;
    float intensity;
    float range;
    float spotAngle;

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

    virtual void setCameraPosition(const Vec3& cameraPos) = 0;
    virtual Vec3 getCameraPosition() const = 0;

    void addLight(const Light& light) {
        m_lights.push_back(light);
    }
    void clearLights() {
        m_lights.clear();
    }

    virtual VertexShaderOutput vertexShader(const VertexShaderInput& input) const;
    virtual Color fragmentShader(const FragmentShaderInput& input) const;

protected:
    Matrix4x4 m_model;
    Matrix4x4 m_view;
    Matrix4x4 m_projection;
    std::vector<Light> m_lights;
};

class FlatShader : public Shader {
public:
    FlatShader(const Color& color);
    Color fragmentShader(const FragmentShaderInput& input) const override;

    void setColor(const Color& color) { m_color = color; }
    void setCameraPosition(const Vec3& cameraPos) override { m_cameraPos = cameraPos; }
    Vec3 getCameraPosition() const override { return m_cameraPos; }

private:
    Color m_color;
    Vec3 m_cameraPos;
};

class PhongShader : public Shader {
public:
    PhongShader();

    void setAmbient(float ambient) { m_ambient = ambient; }
    void setDiffuse(float diffuse) { m_diffuse = diffuse; }
    void setSpecular(float specular) { m_specular = specular; }
    void setShininess(float shininess) { m_shininess = shininess; }
    void setCameraPosition(const Vec3& cameraPos) override { m_cameraPos = cameraPos; }

    Vec3 getCameraPosition() const override { return m_cameraPos; }

    Color fragmentShader(const FragmentShaderInput& input) const override;

private:
    float m_ambient;
    float m_diffuse;
    float m_specular;
    float m_shininess;
    Vec3 m_cameraPos;
};

class ToonShader : public Shader {
public:
    ToonShader();

    void setAmbient(float ambient) { m_ambient = ambient; }
    void setDiffuse(float diffuse) { m_diffuse = diffuse; }
    void setSpecular(float specular) { m_specular = specular; }
    void setShininess(float shininess) { m_shininess = shininess; }
    void setCameraPosition(const Vec3& cameraPos) override { m_cameraPos = cameraPos; }
    void setLevels(int levels) { m_levels = levels; }
    void setOutlineThickness(float thickness) { m_outlineThickness = thickness; }
    void setOutlineColor(const Color& color) { m_outlineColor = color; }
    void setEnableOutline(bool enable) { m_enableOutline = enable; }

    Vec3 getCameraPosition() const override { return m_cameraPos; }

    Color fragmentShader(const FragmentShaderInput& input) const override;

private:
    float m_ambient;
    float m_diffuse;
    float m_specular;
    float m_shininess;
    Vec3 m_cameraPos;
    int m_levels;
    float m_outlineThickness;
    Color m_outlineColor;
    bool m_enableOutline;
};
