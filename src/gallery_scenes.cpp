#include "gallery_scenes.h"
#include "matrix.h"
#include <cmath>
#include <utility>

namespace {
constexpr float pi = 3.14159265358979323846f;

// model = translation * rotationY * scaling: scaled, then turned, then moved.
void place(GalleryScene& scene, Mesh mesh, Vec3 position, Vec3 scale, float yaw = 0) {
    mesh.setModelMatrix(Matrix4x4::translation(position.x, position.y, position.z)
        * Matrix4x4::rotationY(yaw) * Matrix4x4::scaling(scale.x, scale.y, scale.z));
    scene.meshes.push_back(std::move(mesh));
}

// Box centred on p, with the given size and rotation around Y.
void box(GalleryScene& s, Vec3 p, Vec3 size, Color c, float yaw = 0) {
    Mesh m; m.createCube(c); place(s, std::move(m), p, size, yaw);
}

// Sphere centred on p.
void sphere(GalleryScene& s, Vec3 p, float radius, Color c) {
    Mesh m; m.createSphere(48, 32, c);
    place(s, std::move(m), p, Vec3(radius * 2, radius * 2, radius * 2));
}

void face(Mesh& mesh, Vec3 a, Vec3 b, Vec3 c, Color color) {
    Vec3 normal = (b - a).cross(c - a).normalized();
    int i = static_cast<int>(mesh.getVertices().size());
    for (Vec3 p : {a, b, c}) mesh.getVertices().emplace_back(p, normal, Vec2(), color);
    mesh.getTriangles().emplace_back(i, i + 1, i + 2);
}

// Vertical cylinder; p is the centre of its base.
void cylinder(GalleryScene& s, Vec3 p, float radius, float height, Color color, int segments = 48) {
    Mesh m;
    for (int i = 0; i < segments; ++i) {
        float a = 2 * pi * i / segments, b = 2 * pi * (i + 1) / segments;
        Vec3 v0(radius * std::cos(a), 0, radius * std::sin(a));
        Vec3 v1(radius * std::cos(b), 0, radius * std::sin(b));
        Vec3 top(0, height, 0);
        face(m, v0, v0 + top, v1 + top, color); face(m, v0, v1 + top, v1, color);
        face(m, top, v1 + top, v0 + top, color); face(m, Vec3(), v0, v1, color);
    }
    place(s, std::move(m), p, Vec3(1, 1, 1));
}

// Flat ring in XZ, optionally tilted around Z.
void ring(GalleryScene& s, Vec3 p, float radius, float thickness, Color c, float tilt = 0) {
    Mesh m;
    constexpr int segments = 128;
    for (int i = 0; i < segments; ++i) {
        float a = 2 * pi * i / segments, b = 2 * pi * (i + 1) / segments;
        Vec3 v0((radius - thickness) * std::cos(a), 0, (radius - thickness) * std::sin(a));
        Vec3 v1((radius + thickness) * std::cos(a), 0, (radius + thickness) * std::sin(a));
        Vec3 v2((radius + thickness) * std::cos(b), 0, (radius + thickness) * std::sin(b));
        Vec3 v3((radius - thickness) * std::cos(b), 0, (radius - thickness) * std::sin(b));
        face(m, v0, v2, v1, c); face(m, v0, v3, v2, c);
    }
    m.setModelMatrix(Matrix4x4::translation(p.x, p.y, p.z) * Matrix4x4::rotationZ(tilt));
    s.meshes.push_back(std::move(m));
}

// Gable roof; p is the centre of its base.
void roof(GalleryScene& s, Vec3 p, float width, float depth, float height, Color c) {
    Mesh m;
    Vec3 a(-width / 2, 0, -depth / 2), b(width / 2, 0, -depth / 2);
    Vec3 c0(-width / 2, 0, depth / 2), d(width / 2, 0, depth / 2);
    Vec3 e(0, height, -depth / 2), f(0, height, depth / 2);
    face(m, a, e, b, c); face(m, c0, d, f, c);
    face(m, a, c0, f, c); face(m, a, f, e, c);
    face(m, b, e, f, c); face(m, b, f, d, c);
    place(s, std::move(m), p, Vec3(1, 1, 1));
}

// Trunk cylinder and two foliage spheres.
void tree(GalleryScene& s, float x, float z, float size = 1) {
    cylinder(s, Vec3(x, 0.1f, z), 0.09f * size, 0.7f * size, Color(110, 72, 49), 12);
    sphere(s, Vec3(x, 1.0f * size, z), 0.48f * size, Color(62, 119, 91));
    sphere(s, Vec3(x - 0.18f * size, 1.25f * size, z), 0.33f * size, Color(89, 146, 103));
}

void studio(GalleryScene& s) {
    s.background = Color(31, 39, 57); s.eye = Vec3(8, 5.6f, 10); s.target = Vec3(0, -0.1f, 0);
    box(s, Vec3(0, -0.2f, 0), Vec3(7.5f, 0.4f, 5.3f), Color(94, 109, 130));
    cylinder(s, Vec3(-1.8f, 0, 0), 0.95f, 0.35f, Color(166, 177, 190));
    sphere(s, Vec3(-1.8f, 1.25f, 0), 0.9f, Color(222, 91, 72));
    box(s, Vec3(0.35f, 0.85f, 0), Vec3(1.5f, 1.7f, 1.5f), Color(50, 160, 169), -0.28f);
    cylinder(s, Vec3(2.1f, 0, -0.3f), 0.6f, 2.5f, Color(224, 181, 87));
    sphere(s, Vec3(1.35f, 0.4f, 1.6f), 0.4f, Color(168, 144, 193));
}

void village(GalleryScene& s) {
    s.background = Color(213, 231, 234); s.eye = Vec3(10, 8, 12); s.target = Vec3(0, -0.35f, 0);
    box(s, Vec3(0, -0.23f, 0), Vec3(9, 0.45f, 7.5f), Color(120, 157, 115));
    box(s, Vec3(0, 0.015f, 0), Vec3(1.1f, 0.03f, 7.5f), Color(215, 198, 160));
    box(s, Vec3(0, 0.02f, 0), Vec3(7.5f, 0.04f, 0.8f), Color(215, 198, 160));
    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 2; ++col) {
            float x = col ? 2.1f : -2.1f, z = row ? 1.7f : -1.7f;
            Color wall = col ? Color(234, 203, 150) : Color(227, 218, 196);
            box(s, Vec3(x, 0.75f, z), Vec3(1.8f, 1.5f, 1.5f), wall);
            roof(s, Vec3(x, 1.5f, z), 2.1f, 1.9f, 0.8f, Color(164, 77, 61));
            box(s, Vec3(x, 0.45f, z + 0.756f), Vec3(0.37f, 0.9f, 0.035f), Color(90, 66, 52));
            for (float dx : {-0.57f, 0.57f})
                box(s, Vec3(x + dx, 0.95f, z + 0.76f), Vec3(0.32f, 0.38f, 0.04f), Color(72, 124, 141));
            box(s, Vec3(x + 0.5f, 2.05f, z - 0.3f), Vec3(0.25f, 0.9f, 0.3f), Color(126, 93, 81));
        }
    }
    for (float x : {-3.6f, 3.6f})
        for (float z : {-2.6f, 0.0f, 2.6f})
            tree(s, x, z, 0.85f);
}

void temple(GalleryScene& s) {
    s.background = Color(228, 219, 205); s.eye = Vec3(9, 6.5f, 11); s.target = Vec3(0, 0.65f, 0);
    box(s, Vec3(0, -0.25f, 0), Vec3(8, 0.5f, 6), Color(175, 156, 129));
    for (int i = 0; i < 3; ++i)
        box(s, Vec3(0, 0.12f + i * 0.22f, 0), Vec3(6.4f - i * 0.4f, 0.24f, 4.6f - i * 0.4f), Color(210, 197, 171));
    for (float x : {-2.2f, -0.75f, 0.75f, 2.2f}) {
        for (float z : {-1.35f, 1.35f}) {
            cylinder(s, Vec3(x, 0.65f, z), 0.33f, 0.15f, Color(226, 216, 193));
            cylinder(s, Vec3(x, 0.8f, z), 0.22f, 2.2f, Color(233, 222, 198), 20);
            box(s, Vec3(x, 3.05f, z), Vec3(0.66f, 0.2f, 0.66f), Color(223, 211, 187));
        }
    }
    box(s, Vec3(0, 3.3f, 0), Vec3(5.5f, 0.3f, 3.6f), Color(222, 207, 178));
    roof(s, Vec3(0, 3.45f, 0), 5.8f, 3.9f, 0.85f, Color(160, 91, 69));
    cylinder(s, Vec3(0, 0.7f, 0), 0.55f, 0.4f, Color(177, 159, 128));
    sphere(s, Vec3(0, 1.55f, 0), 0.45f, Color(179, 126, 57));
}

void jardin(GalleryScene& s) {
    s.background = Color(217, 231, 219); s.eye = Vec3(9, 8, 11); s.target = Vec3(0, -0.35f, 0);
    box(s, Vec3(0, -0.2f, 0), Vec3(8, 0.4f, 7), Color(129, 159, 116));
    box(s, Vec3(0, 0.025f, 0), Vec3(7.6f, 0.05f, 1.5f), Color(225, 210, 178));
    box(s, Vec3(0, 0.03f, 0), Vec3(1.6f, 0.06f, 6.6f), Color(225, 210, 178));
    cylinder(s, Vec3(0, 0.05f, 0), 1.45f, 0.3f, Color(176, 181, 164));
    cylinder(s, Vec3(0, 0.36f, 0), 1.22f, 0.035f, Color(66, 150, 170));
    cylinder(s, Vec3(0, 0.4f, 0), 0.24f, 1.0f, Color(200, 202, 180));
    cylinder(s, Vec3(0, 1.35f, 0), 0.7f, 0.13f, Color(205, 208, 187));
    cylinder(s, Vec3(0, 1.49f, 0), 0.6f, 0.025f, Color(79, 168, 183));
    sphere(s, Vec3(0, 1.78f, 0), 0.23f, Color(208, 211, 188));
    for (float x : {-2.7f, 2.7f}) {
        for (float z : {-2.1f, 2.1f}) {
            box(s, Vec3(x, 0.15f, z), Vec3(1.5f, 0.3f, 1.45f), Color(101, 121, 86));
            tree(s, x, z, 1.2f);
            for (float dx : {-0.5f, 0.5f})
                sphere(s, Vec3(x + dx, 0.42f, z + 0.45f), 0.14f, Color(226, 155, 136));
        }
    }
    for (float x : {-2.65f, 2.65f}) {
        box(s, Vec3(x, 0.5f, 0), Vec3(1.3f, 0.14f, 0.5f), Color(157, 101, 61));
        for (float dx : {-0.45f, 0.45f})
            box(s, Vec3(x + dx, 0.26f, 0), Vec3(0.1f, 0.45f, 0.4f), Color(75, 89, 76));
    }
}

void orbites(GalleryScene& s) {
    s.background = Color(14, 21, 39); s.eye = Vec3(8, 10, 13); s.target = Vec3(0, 0, 0);
    s.ambient = 0.5f;
    sphere(s, Vec3(), 0.85f, Color(255, 188, 65));
    const Color colors[] = {
        Color(187, 174, 157), Color(225, 148, 87), Color(65, 145, 196),
        Color(200, 85, 65), Color(207, 182, 130)};
    const float angles[] = {0.3f, 2.2f, 4.7f, 3.3f, 0.5f};
    const float radii[] = {0.14f, 0.23f, 0.29f, 0.24f, 0.52f};
    for (int i = 0; i < 5; ++i) {
        float r = 1.35f + i * 0.85f;
        ring(s, Vec3(), r, 0.012f, Color(75, 94, 121));
        Vec3 p(r * std::cos(angles[i]), 0, r * std::sin(angles[i]));
        sphere(s, p, radii[i], colors[i]);
        if (i == 4)
            ring(s, p, 0.85f, 0.18f, Color(180, 158, 115), 0.3f);
    }
}
}  // namespace

GalleryScene makeGalleryScene(const std::string& name) {
    GalleryScene s{name, Color(224, 231, 235), Vec3(9, 7, 11), Vec3(0, -0.35f, 0), 0.42f, {}};
    if (name == "studio") studio(s);
    else if (name == "village") village(s);
    else if (name == "temple") temple(s);
    else if (name == "jardin") jardin(s);
    else if (name == "orbites") orbites(s);
    return s;
}
