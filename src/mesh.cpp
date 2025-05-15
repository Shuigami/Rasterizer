#include "mesh.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

Mesh::Mesh() {
}

Mesh::~Mesh() {
}

bool Mesh::loadFromOBJ(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    std::vector<Vec3> positions;
    std::vector<Vec3> normals;
    std::vector<Vec2> texCoords;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            positions.push_back(Vec3(x, y, z));
        }
        else if (token == "vn") {
            float x, y, z;
            iss >> x >> y >> z;
            normals.push_back(Vec3(x, y, z));
        }
        else if (token == "vt") {
            float u, v;
            iss >> u >> v;
            texCoords.push_back(Vec2(u, v));
        }
        else if (token == "f") {
            std::string vertexData;
            std::vector<int> positionIndices, texCoordIndices, normalIndices;

            while (iss >> vertexData) {
                std::istringstream vertexStream(vertexData);
                std::string indexStr;

                std::getline(vertexStream, indexStr, '/');
                if (!indexStr.empty()) {
                    positionIndices.push_back(std::stoi(indexStr) - 1);
                }

                std::getline(vertexStream, indexStr, '/');
                if (!indexStr.empty()) {
                    texCoordIndices.push_back(std::stoi(indexStr) - 1);
                }

                std::getline(vertexStream, indexStr, '/');
                if (!indexStr.empty()) {
                    normalIndices.push_back(std::stoi(indexStr) - 1);
                }
            }

            if (positionIndices.size() >= 3) {
                for (size_t i = 2; i < positionIndices.size(); ++i) {
                    Vertex v1, v2, v3;

                    v1.position = positions[positionIndices[0]];
                    if (!texCoordIndices.empty() && texCoordIndices[0] < texCoords.size()) {
                        v1.texCoord = texCoords[texCoordIndices[0]];
                    }
                    if (!normalIndices.empty() && normalIndices[0] < normals.size()) {
                        v1.normal = normals[normalIndices[0]];
                    }
                    v1.color = Color(255, 255, 255);

                    v2.position = positions[positionIndices[i-1]];
                    if (!texCoordIndices.empty() && texCoordIndices[i-1] < texCoords.size()) {
                        v2.texCoord = texCoords[texCoordIndices[i-1]];
                    }
                    if (!normalIndices.empty() && normalIndices[i-1] < normals.size()) {
                        v2.normal = normals[normalIndices[i-1]];
                    }
                    v2.color = Color(255, 255, 255);

                    v3.position = positions[positionIndices[i]];
                    if (!texCoordIndices.empty() && texCoordIndices[i] < texCoords.size()) {
                        v3.texCoord = texCoords[texCoordIndices[i]];
                    }
                    if (!normalIndices.empty() && normalIndices[i] < normals.size()) {
                        v3.normal = normals[normalIndices[i]];
                    }
                    v3.color = Color(255, 255, 255);

                    int v1Index = m_vertices.size();
                    m_vertices.push_back(v1);

                    int v2Index = m_vertices.size();
                    m_vertices.push_back(v2);

                    int v3Index = m_vertices.size();
                    m_vertices.push_back(v3);

                    m_triangles.push_back(Triangle(v1Index, v2Index, v3Index));
                }
            }
        }
    }

    if (normals.empty()) {
        generateNormals();
    }

    return true;
}

void Mesh::createCube() {
    m_vertices.clear();
    m_triangles.clear();

    Vec3 vertices[8] = {
        Vec3(-0.5f, -0.5f, -0.5f),
        Vec3(0.5f, -0.5f, -0.5f),
        Vec3(0.5f, 0.5f, -0.5f),
        Vec3(-0.5f, 0.5f, -0.5f),
        Vec3(-0.5f, -0.5f, 0.5f),
        Vec3(0.5f, -0.5f, 0.5f),
        Vec3(0.5f, 0.5f, 0.5f),
        Vec3(-0.5f, 0.5f, 0.5f)
    };

    Vec3 normals[6] = {
        Vec3(0.0f, 0.0f, -1.0f),
        Vec3(0.0f, 0.0f, 1.0f),
        Vec3(1.0f, 0.0f, 0.0f),
        Vec3(-1.0f, 0.0f, 0.0f),
        Vec3(0.0f, 1.0f, 0.0f),
        Vec3(0.0f, -1.0f, 0.0f)
    };

    Vec2 texCoords[4] = {
        Vec2(0.0f, 0.0f),
        Vec2(1.0f, 0.0f),
        Vec2(1.0f, 1.0f),
        Vec2(0.0f, 1.0f)
    };

    int faces[6][4] = {
        {0, 3, 2, 1},
        {4, 5, 6, 7},
        {1, 2, 6, 5},
        {0, 4, 7, 3},
        {3, 7, 6, 2},
        {0, 1, 5, 4}
    };

    for (int f = 0; f < 6; ++f) {
        int baseIndex = m_vertices.size();

        for (int v = 0; v < 4; ++v) {
            Vertex vertex;
            vertex.position = vertices[faces[f][v]];
            vertex.normal = normals[f];
            vertex.texCoord = texCoords[v];
            vertex.color = Color(255, 255, 255);
            m_vertices.push_back(vertex);
        }

        m_triangles.push_back(Triangle(baseIndex, baseIndex + 1, baseIndex + 2));
        m_triangles.push_back(Triangle(baseIndex, baseIndex + 2, baseIndex + 3));
    }
}

void Mesh::createSphere(int slices, int stacks) {
    m_vertices.clear();
    m_triangles.clear();

    float radius = 0.5f;

    for (int stack = 0; stack <= stacks; ++stack) {
        float phi = M_PI * float(stack) / float(stacks);
        float sinPhi = std::sin(phi);
        float cosPhi = std::cos(phi);

        for (int slice = 0; slice <= slices; ++slice) {
            float theta = 2.0f * M_PI * float(slice) / float(slices);
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);

            float x = cosTheta * sinPhi;
            float y = cosPhi;
            float z = sinTheta * sinPhi;

            Vertex vertex;
            vertex.position = Vec3(x, y, z) * radius;
            vertex.normal = Vec3(x, y, z).normalized();
            vertex.texCoord = Vec2(float(slice) / float(slices), float(stack) / float(stacks));
            vertex.color = Color(255, 255, 255);

            m_vertices.push_back(vertex);
        }
    }

    for (int stack = 0; stack < stacks; ++stack) {
        for (int slice = 0; slice < slices; ++slice) {
            int topLeft = stack * (slices + 1) + slice;
            int topRight = topLeft + 1;
            int bottomLeft = (stack + 1) * (slices + 1) + slice;
            int bottomRight = bottomLeft + 1;

            m_triangles.push_back(Triangle(topLeft, bottomLeft, topRight));
            m_triangles.push_back(Triangle(topRight, bottomLeft, bottomRight));
        }
    }
}

void Mesh::generateNormals() {
    for (auto& vertex : m_vertices) {
        vertex.normal = Vec3(0.0f, 0.0f, 0.0f);
    }

    for (const auto& triangle : m_triangles) {
        const Vec3& v1 = m_vertices[triangle.v1].position;
        const Vec3& v2 = m_vertices[triangle.v2].position;
        const Vec3& v3 = m_vertices[triangle.v3].position;

        Vec3 edge1 = v2 - v1;
        Vec3 edge2 = v3 - v1;
        Vec3 normal = edge1.cross(edge2).normalized();

        m_vertices[triangle.v1].normal = m_vertices[triangle.v1].normal + normal;
        m_vertices[triangle.v2].normal = m_vertices[triangle.v2].normal + normal;
        m_vertices[triangle.v3].normal = m_vertices[triangle.v3].normal + normal;
    }

    for (auto& vertex : m_vertices) {
        vertex.normal = vertex.normal.normalized();
    }
}
