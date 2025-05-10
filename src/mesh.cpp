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
            // Vertex position
            float x, y, z;
            iss >> x >> y >> z;
            positions.push_back(Vec3(x, y, z));
        }
        else if (token == "vn") {
            // Vertex normal
            float x, y, z;
            iss >> x >> y >> z;
            normals.push_back(Vec3(x, y, z));
        }
        else if (token == "vt") {
            // Texture coordinate
            float u, v;
            iss >> u >> v;
            texCoords.push_back(Vec2(u, v));
        }
        else if (token == "f") {
            // Face
            std::string vertexData;
            std::vector<int> positionIndices, texCoordIndices, normalIndices;

            while (iss >> vertexData) {
                // OBJ format: v/vt/vn or v//vn or v/vt or just v
                std::istringstream vertexStream(vertexData);
                std::string indexStr;

                // Position index
                std::getline(vertexStream, indexStr, '/');
                if (!indexStr.empty()) {
                    positionIndices.push_back(std::stoi(indexStr) - 1); // OBJ indices start at 1
                }

                // Texture coordinate index
                std::getline(vertexStream, indexStr, '/');
                if (!indexStr.empty()) {
                    texCoordIndices.push_back(std::stoi(indexStr) - 1);
                }

                // Normal index
                std::getline(vertexStream, indexStr, '/');
                if (!indexStr.empty()) {
                    normalIndices.push_back(std::stoi(indexStr) - 1);
                }
            }

            // Create vertices and triangles
            // For simplicity, we'll assume triangulated faces
            if (positionIndices.size() >= 3) {
                for (size_t i = 2; i < positionIndices.size(); ++i) {
                    // Create vertices
                    Vertex v1, v2, v3;

                    // First vertex
                    v1.position = positions[positionIndices[0]];
                    if (!texCoordIndices.empty() && texCoordIndices[0] < texCoords.size()) {
                        v1.texCoord = texCoords[texCoordIndices[0]];
                    }
                    if (!normalIndices.empty() && normalIndices[0] < normals.size()) {
                        v1.normal = normals[normalIndices[0]];
                    }
                    v1.color = Color(255, 255, 255);

                    // Second vertex
                    v2.position = positions[positionIndices[i-1]];
                    if (!texCoordIndices.empty() && texCoordIndices[i-1] < texCoords.size()) {
                        v2.texCoord = texCoords[texCoordIndices[i-1]];
                    }
                    if (!normalIndices.empty() && normalIndices[i-1] < normals.size()) {
                        v2.normal = normals[normalIndices[i-1]];
                    }
                    v2.color = Color(255, 255, 255);

                    // Third vertex
                    v3.position = positions[positionIndices[i]];
                    if (!texCoordIndices.empty() && texCoordIndices[i] < texCoords.size()) {
                        v3.texCoord = texCoords[texCoordIndices[i]];
                    }
                    if (!normalIndices.empty() && normalIndices[i] < normals.size()) {
                        v3.normal = normals[normalIndices[i]];
                    }
                    v3.color = Color(255, 255, 255);

                    // Add vertices to the mesh
                    int v1Index = m_vertices.size();
                    m_vertices.push_back(v1);

                    int v2Index = m_vertices.size();
                    m_vertices.push_back(v2);

                    int v3Index = m_vertices.size();
                    m_vertices.push_back(v3);

                    // Add triangle to the mesh
                    m_triangles.push_back(Triangle(v1Index, v2Index, v3Index));
                }
            }
        }
    }

    // If no normals were provided, generate them
    if (normals.empty()) {
        generateNormals();
    }

    return true;
}

void Mesh::createCube() {
    m_vertices.clear();
    m_triangles.clear();

    // Define the 8 vertices of the cube
    Vec3 vertices[8] = {
        Vec3(-0.5f, -0.5f, -0.5f), // 0: left bottom back
        Vec3(0.5f, -0.5f, -0.5f),  // 1: right bottom back
        Vec3(0.5f, 0.5f, -0.5f),   // 2: right top back
        Vec3(-0.5f, 0.5f, -0.5f),  // 3: left top back
        Vec3(-0.5f, -0.5f, 0.5f),  // 4: left bottom front
        Vec3(0.5f, -0.5f, 0.5f),   // 5: right bottom front
        Vec3(0.5f, 0.5f, 0.5f),    // 6: right top front
        Vec3(-0.5f, 0.5f, 0.5f)    // 7: left top front
    };

    // Define the 6 face normals
    Vec3 normals[6] = {
        Vec3(0.0f, 0.0f, -1.0f),  // back
        Vec3(0.0f, 0.0f, 1.0f),   // front
        Vec3(1.0f, 0.0f, 0.0f),   // right
        Vec3(-1.0f, 0.0f, 0.0f),  // left
        Vec3(0.0f, 1.0f, 0.0f),   // top
        Vec3(0.0f, -1.0f, 0.0f)   // bottom
    };

    // Define the texture coordinates
    Vec2 texCoords[4] = {
        Vec2(0.0f, 0.0f),  // bottom-left
        Vec2(1.0f, 0.0f),  // bottom-right
        Vec2(1.0f, 1.0f),  // top-right
        Vec2(0.0f, 1.0f)   // top-left
    };

    // Define the 12 triangles (2 per face)
    int faces[6][4] = {
        {0, 1, 2, 3},  // back face (0,1,2,3)
        {4, 7, 6, 5},  // front face (4,7,6,5)
        {1, 5, 6, 2},  // right face (1,5,6,2)
        {0, 3, 7, 4},  // left face (0,3,7,4)
        {3, 2, 6, 7},  // top face (3,2,6,7)
        {0, 4, 5, 1}   // bottom face (0,4,5,1)
    };

    // Create all vertices and triangles
    for (int f = 0; f < 6; ++f) {
        int baseIndex = m_vertices.size();

        // Add the 4 vertices for this face
        for (int v = 0; v < 4; ++v) {
            Vertex vertex;
            vertex.position = vertices[faces[f][v]];
            vertex.normal = normals[f];
            vertex.texCoord = texCoords[v];
            vertex.color = Color(255, 255, 255);
            m_vertices.push_back(vertex);
        }

        // Add the 2 triangles for this face
        m_triangles.push_back(Triangle(baseIndex, baseIndex + 1, baseIndex + 2));
        m_triangles.push_back(Triangle(baseIndex, baseIndex + 2, baseIndex + 3));
    }
}

void Mesh::createSphere(int slices, int stacks) {
    m_vertices.clear();
    m_triangles.clear();

    float radius = 0.5f;

    // Create vertices
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

    // Create triangles
    for (int stack = 0; stack < stacks; ++stack) {
        for (int slice = 0; slice < slices; ++slice) {
            int topLeft = stack * (slices + 1) + slice;
            int topRight = topLeft + 1;
            int bottomLeft = (stack + 1) * (slices + 1) + slice;
            int bottomRight = bottomLeft + 1;

            // Create two triangles per quad
            m_triangles.push_back(Triangle(topLeft, bottomLeft, topRight));
            m_triangles.push_back(Triangle(topRight, bottomLeft, bottomRight));
        }
    }
}

void Mesh::generateNormals() {
    // Initialize all normals to zero
    for (auto& vertex : m_vertices) {
        vertex.normal = Vec3(0.0f, 0.0f, 0.0f);
    }

    // Compute face normals and accumulate them on vertices
    for (const auto& triangle : m_triangles) {
        const Vec3& v1 = m_vertices[triangle.v1].position;
        const Vec3& v2 = m_vertices[triangle.v2].position;
        const Vec3& v3 = m_vertices[triangle.v3].position;

        // Compute face normal
        Vec3 edge1 = v2 - v1;
        Vec3 edge2 = v3 - v1;
        Vec3 normal = edge1.cross(edge2).normalized();

        // Accumulate normal to vertices
        m_vertices[triangle.v1].normal = m_vertices[triangle.v1].normal + normal;
        m_vertices[triangle.v2].normal = m_vertices[triangle.v2].normal + normal;
        m_vertices[triangle.v3].normal = m_vertices[triangle.v3].normal + normal;
    }

    // Normalize all normals
    for (auto& vertex : m_vertices) {
        vertex.normal = vertex.normal.normalized();
    }
}
