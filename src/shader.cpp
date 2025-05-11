#include "shader.h"
#include <cmath>
#include <algorithm>

Shader::Shader() {
}

Shader::~Shader() {
}

VertexShaderOutput Shader::vertexShader(const VertexShaderInput& input) const {
    VertexShaderOutput output;

    // Transform the position to clip space
    Vec4 worldPos = m_model * Vec4(input.position, 1.0f);
    Vec4 viewPos = m_view * worldPos;
    output.position = m_projection * viewPos;
    
    // Ensure proper Z values for depth testing
    if (output.position.w != 0.0f) {
        output.position.z = std::max(0.0f, std::min(output.position.w, output.position.z));
    }

    // Transform the normal to world space (ignoring translation)
    Matrix4x4 normalMatrix = m_model; // This should be the inverse transpose of the model matrix for non-uniform scaling
    Vec4 transformedNormal = normalMatrix * Vec4(input.normal, 0.0f);
    output.normal = Vec3(transformedNormal.x, transformedNormal.y, transformedNormal.z).normalized();

    // Save world position for lighting calculations
    output.worldPos = Vec3(worldPos.x, worldPos.y, worldPos.z);

    // Pass through texture coordinates and color
    output.texCoord = input.texCoord;
    output.color = input.color;

    return output;
}

Color Shader::fragmentShader(const FragmentShaderInput& input) const {
    // Default implementation: just return the interpolated color
    return input.color;
}

// FlatShader implementation
FlatShader::FlatShader(const Color& color) : m_color(color), m_cameraPos(0.0f, 0.0f, 5.0f) {
}

Color FlatShader::fragmentShader(const FragmentShaderInput& input) const {
    return m_color;
}

// TextureShader implementation
TextureShader::TextureShader() : m_cameraPos(0.0f, 0.0f, 5.0f) {
}

Color TextureShader::fragmentShader(const FragmentShaderInput& input) const {
    if (m_texture) {
        return m_texture->sample(input.texCoord.x, input.texCoord.y);
    }
    return input.color;
}

// PhongShader implementation
PhongShader::PhongShader()
    : m_ambient(0.2f), m_diffuse(0.7f), m_specular(0.5f), m_shininess(32.0f) {
}

Color PhongShader::fragmentShader(const FragmentShaderInput& input) const {
    // Get base color from texture or vertex color
    Color baseColor;
    if (m_texture) {
        baseColor = m_texture->sample(input.texCoord.x, input.texCoord.y);
    } else {
        baseColor = input.color;
    }

    // Ambient component
    Color ambientColor = baseColor * m_ambient;

    // View direction (normalized)
    Vec3 viewDir = (m_cameraPos - input.worldPos).normalized();

    // Initialize result with ambient light
    Color result = ambientColor;

    // Process all lights
    for (const auto& light : m_lights) {
        // Compute light direction and distance
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

                // Compute attenuation
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

                // Compute cone attenuation
                float cosAngle = -lightDir.dot(light.direction.normalized());
                float spotFactor = 0.0f;

                if (cosAngle > std::cos(light.spotAngle)) {
                    spotFactor = std::pow(cosAngle, 4.0f); // Sharpen the spotlight edge
                }

                // Compute distance attenuation
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

        // Skip if no contribution from this light
        if (attenuation <= 0.0f) {
            continue;
        }

        // Diffuse term: (N·L)
        float diffuseFactor = std::max(0.0f, input.normal.dot(lightDir));
        Color diffuse = baseColor * (diffuseFactor * m_diffuse * light.intensity * attenuation);

        // Specular term: (R·V)^shininess
        Color specular(0, 0, 0);
        if (diffuseFactor > 0.0f) {
            Vec3 reflectDir = (input.normal * (2.0f * input.normal.dot(lightDir)) - lightDir).normalized();
            float specularFactor = std::pow(std::max(0.0f, viewDir.dot(reflectDir)), m_shininess);
            specular = Color(255, 255, 255) * (specularFactor * m_specular * light.intensity * attenuation);
        }

        // Apply light color
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

        // Add contribution from this light
        result = result + diffuse + specular;
    }

    return result;
}
