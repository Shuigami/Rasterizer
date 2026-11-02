#pragma once

#include <string>
#include <vector>
#include "mesh.h"
#include "vector.h"

// Static, procedural scenes shown in the portfolio gallery: geometric shapes
// only, no OBJ models or textures. Ground is the XZ plane, Y is up.
struct GalleryScene {
    std::string name;
    Color background;
    Vec3 eye;
    Vec3 target;
    float ambient;
    std::vector<Mesh> meshes;
};

// Names: "studio", "village", "temple", "jardin", "orbites".
GalleryScene makeGalleryScene(const std::string& name);

// Shared look of every gallery scene.
constexpr float GALLERY_FOV_DEGREES = 40.0f;
