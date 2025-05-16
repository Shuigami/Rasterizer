#include "shader.h"
#include <cmath>
#include <algorithm>
#include <iostream>

Shader::Shader() {
}

Shader::~Shader() {
}

VertexShaderOutput Shader::vertexShader(const VertexShaderInput& input) const {
    VertexShaderOutput output;
    std::cout << "Projection Matrix: " << std::endl
              << m_projection(0, 0) << ", " << m_projection(0, 1) << ", " << m_projection(0, 2) << ", " << m_projection(0, 3) << std::endl
              << m_projection(1, 0) << ", " << m_projection(1, 1) << ", " << m_projection(1, 2) << ", " << m_projection(1, 3) << std::endl
              << m_projection(2, 0) << ", " << m_projection(2, 1) << ", " << m_projection(2, 2) << ", " << m_projection(2, 3) << std::endl
              << m_projection(3, 0) << ", " << m_projection(3, 1) << ", " << m_projection(3, 2) << ", " << m_projection(3, 3) << std::endl;

    Vec4 worldPos = m_model * Vec4(input.position, 1.0f);
    std::cout << "World Position: "
              << "x: " << worldPos.x << ", "
              << "y: " << worldPos.y << ", "
              << "z: " << worldPos.z << ", "
              << "w: " << worldPos.w << std::endl;
    Vec4 viewPos = m_view * worldPos;
    std::cout << "View Position: "
              << "x: " << viewPos.x << ", "
              << "y: " << viewPos.y << ", "
              << "z: " << viewPos.z << ", "
              << "w: " << viewPos.w << std::endl;
    output.position = m_projection * viewPos;
    std::cout << "Clip Position: "
              << "x: " << output.position.x << ", "
              << "y: " << output.position.y << ", "
              << "z: " << output.position.z << ", "
              << "w: " << output.position.w << std::endl;

    Matrix4x4 normalMatrix = m_model;
    Vec4 transformedNormal = normalMatrix * Vec4(input.normal, 0.0f);
    output.normal = Vec3(transformedNormal.x, transformedNormal.y, transformedNormal.z).normalized();

    output.worldPos = Vec3(worldPos.x, worldPos.y, worldPos.z);

    output.texCoord = input.texCoord;
    output.color = input.color;

    return output;
}

Color Shader::fragmentShader(const FragmentShaderInput& input) const {
    return input.color;
}

FlatShader::FlatShader(const Color& color) : m_color(color), m_cameraPos(0.0f, 0.0f, 5.0f) {
}

Color FlatShader::fragmentShader(const FragmentShaderInput& input) const {
    if (m_color.r == 255 && m_color.g == 255 && m_color.b == 255) {
        return input.color;
    }
    return m_color;
}

PhongShader::PhongShader()
    : m_ambient(0.2f), m_diffuse(0.7f), m_specular(0.5f), m_shininess(32.0f) {
}

Color PhongShader::fragmentShader(const FragmentShaderInput& input) const {
    Color baseColor = input.color;

    Color ambientColor = baseColor * m_ambient;

    Vec3 viewDir = (m_cameraPos - input.worldPos).normalized();

    Color result = ambientColor;

    for (const auto& light : m_lights) {
        Vec3 lightDir;
        float attenuation = 1.0f;

        switch (light.type) {
            case Light::Type::Directional:
                lightDir = -light.direction.normalized();
                break;

            case Light::Type::Point: {
                Vec3 lightVec = light.position - input.worldPos;
                float distance = lightVec.length();
                lightDir = lightVec.normalized();

                if (distance > light.range) {
                    attenuation = 0.0f;
                } else {
                    float att = 1.0f - (distance / light.range);
                    attenuation = att * att;
                }
                break;
            }

            case Light::Type::Spot: {
                Vec3 lightVec = light.position - input.worldPos;
                float distance = lightVec.length();
                lightDir = lightVec.normalized();

                float cosAngle = -lightDir.dot(light.direction.normalized());
                float spotFactor = 0.0f;

                if (cosAngle > std::cos(light.spotAngle)) {
                    spotFactor = std::pow(cosAngle, 4.0f);
                }

                float distanceAtt = 1.0f;
                if (distance > light.range) {
                    distanceAtt = 0.0f;
                } else {
                    float att = 1.0f - (distance / light.range);
                    distanceAtt = att * att;
                }

                attenuation = spotFactor * distanceAtt;
                break;
            }
        }

        if (attenuation <= 0.0f) {
            continue;
        }

        float diffuseFactor = std::max(0.0f, input.normal.dot(lightDir));
        Color diffuse = baseColor * (diffuseFactor * m_diffuse * light.intensity * attenuation);

        Color specular(0, 0, 0);
        if (diffuseFactor > 0.0f) {
            Vec3 reflectDir = (input.normal * (2.0f * input.normal.dot(lightDir)) - lightDir).normalized();
            float specularFactor = std::pow(std::max(0.0f, viewDir.dot(reflectDir)), m_shininess);
            specular = Color(255, 255, 255) * (specularFactor * m_specular * light.intensity * attenuation);
        }

        diffuse = Color(
            static_cast<uint8_t>(std::min(255.0f, (diffuse.r * light.color.r) / 255.0f)),
            static_cast<uint8_t>(std::min(255.0f, (diffuse.g * light.color.g) / 255.0f)),
            static_cast<uint8_t>(std::min(255.0f, (diffuse.b * light.color.b) / 255.0f)),
            diffuse.a
        );

        specular = Color(
            static_cast<uint8_t>(std::min(255.0f, (specular.r * light.color.r) / 255.0f)),
            static_cast<uint8_t>(std::min(255.0f, (specular.g * light.color.g) / 255.0f)),
            static_cast<uint8_t>(std::min(255.0f, (specular.b * light.color.b) / 255.0f)),
            specular.a
        );

        result = result + diffuse + specular;
    }

    return result;
}

ToonShader::ToonShader()
    : m_ambient(0.2f), m_diffuse(0.8f), m_specular(0.5f), m_shininess(32.0f),
      m_levels(4), m_outlineThickness(0.3f), m_outlineColor(0, 0, 0), m_enableOutline(true) {
}

Color ToonShader::fragmentShader(const FragmentShaderInput& input) const {
    Color baseColor = input.color;

    Color ambientColor = baseColor * m_ambient;

    Vec3 viewDir = (m_cameraPos - input.worldPos).normalized();

    Color result = ambientColor;

    if (m_enableOutline) {
        float edgeFactor = input.normal.dot(viewDir);
        if (edgeFactor < m_outlineThickness) {
            return m_outlineColor;
        }
    }

    for (const auto& light : m_lights) {
        Vec3 lightDir;
        float attenuation = 1.0f;

        switch (light.type) {
            case Light::Type::Directional:
                lightDir = -light.direction.normalized();
                break;

            case Light::Type::Point: {
                Vec3 lightVec = light.position - input.worldPos;
                float distance = lightVec.length();
                lightDir = lightVec.normalized();

                if (distance > light.range) {
                    attenuation = 0.0f;
                } else {
                    float att = 1.0f - (distance / light.range);
                    attenuation = att * att;
                }
                break;
            }

            case Light::Type::Spot: {
                Vec3 lightVec = light.position - input.worldPos;
                float distance = lightVec.length();
                lightDir = lightVec.normalized();

                float cosAngle = -lightDir.dot(light.direction.normalized());
                float spotFactor = 0.0f;

                if (cosAngle > std::cos(light.spotAngle)) {
                    spotFactor = std::pow(cosAngle, 4.0f);
                }

                float distanceAtt = 1.0f;
                if (distance > light.range) {
                    distanceAtt = 0.0f;
                } else {
                    float att = 1.0f - (distance / light.range);
                    distanceAtt = att * att;
                }

                attenuation = spotFactor * distanceAtt;
                break;
            }
        }

        if (attenuation <= 0.0f) {
            continue;
        }

        float diffuseFactor = std::max(0.0f, input.normal.dot(lightDir));

        if (diffuseFactor > 0.0f) {
            diffuseFactor = std::ceil(diffuseFactor * m_levels) / m_levels;
        }

        Color diffuse = baseColor * (diffuseFactor * m_diffuse * light.intensity * attenuation);

        Color specular(0, 0, 0);
        if (diffuseFactor > 0.0f) {
            Vec3 reflectDir = (input.normal * (2.0f * input.normal.dot(lightDir)) - lightDir).normalized();
            float specularFactor = std::pow(std::max(0.0f, viewDir.dot(reflectDir)), m_shininess);

            if (specularFactor > 0.7f) {
                specularFactor = 1.0f;
            } else {
                specularFactor = 0.0f;
            }

            specular = Color(255, 255, 255) * (specularFactor * m_specular * light.intensity * attenuation);
        }

        diffuse = Color(
            static_cast<uint8_t>(std::min(255.0f, (diffuse.r * light.color.r) / 255.0f)),
            static_cast<uint8_t>(std::min(255.0f, (diffuse.g * light.color.g) / 255.0f)),
            static_cast<uint8_t>(std::min(255.0f, (diffuse.b * light.color.b) / 255.0f)),
            diffuse.a
        );

        specular = Color(
            static_cast<uint8_t>(std::min(255.0f, (specular.r * light.color.r) / 255.0f)),
            static_cast<uint8_t>(std::min(255.0f, (specular.g * light.color.g) / 255.0f)),
            static_cast<uint8_t>(std::min(255.0f, (specular.b * light.color.b) / 255.0f)),
            specular.a
        );

        result = result + diffuse + specular;
    }

    return result;
}
