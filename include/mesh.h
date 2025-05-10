#pragma once

#include <vector>
#include <memory>
#include <string>
#include "vector.h"
#include "texture.h"

struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec2 texCoord;
    Color color;

    Vertex() : position(), normal(), texCoord(), color() {}

    Vertex(const Vec3& pos, const Vec3& norm, const Vec2& tex, const Color& col)
        : position(pos), normal(norm), texCoord(tex), color(col) {}
};

struct Triangle {
    int v1, v2, v3;

    Triangle() : v1(0), v2(0), v3(0) {}
    Triangle(int a, int b, int c) : v1(a), v2(b), v3(c) {}
};

class Mesh {
public:
    Mesh();
    ~Mesh();

    bool loadFromOBJ(const std::string& filename);
    void createCube();
    void createSphere(int slices, int stacks);
    void generateNormals();

    const std::vector<Vertex>& getVertices() const { return m_vertices; }
    const std::vector<Triangle>& getTriangles() const { return m_triangles; }

    void setTexture(const std::shared_ptr<Texture>& texture) { m_texture = texture; }
    std::shared_ptr<Texture> getTexture() const { return m_texture; }

private:
    std::vector<Vertex> m_vertices;
    std::vector<Triangle> m_triangles;
    std::shared_ptr<Texture> m_texture;
};
