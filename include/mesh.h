#pragma once

#include <vector>
#include <string>
#include "vector.h"

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
    void createCube(const Color& color = Color(255, 255, 255));
    void createSphere(int slices, int stacks, const Color& color = Color(255, 255, 255));
    void createPlane(float width, float depth, const Color& color = Color(255, 255, 255));
    void generateNormals();

    void setVertexColor(int index, const Color& color);
    void setAllVertexColors(const Color& color);
    void setFaceColor(int triangleIndex, const Color& color);

    void setVertexColorsFromPosition();
    void setRandomVertexColors();
    void setGradientColors(const Color& startColor, const Color& endColor, bool verticalGradient = true);

    const std::vector<Vertex>& getVertices() const { return m_vertices; }
    std::vector<Vertex>& getVertices() { return m_vertices; }
    const std::vector<Triangle>& getTriangles() const { return m_triangles; }
    std::vector<Triangle>& getTriangles() { return m_triangles; }

private:
    std::vector<Vertex> m_vertices;
    std::vector<Triangle> m_triangles;
};
