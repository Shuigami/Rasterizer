#pragma once

#include "geometry/point.hh"
#include "geometry/vector.hh"
#include "image/color.hh"
#include "texture/texture_material.hh"

#include <map>

class Object {
public:
    Object(TextureMaterial* texture);
    virtual bool intersect(const Point3& p, const Vector3& vect, double &t) const = 0;
    virtual Vector3 normal(const Point3& p) const = 0;
    std::map<std::string, double> getTextureInfo() const;
    Color getColor();

private:
    TextureMaterial* texture_;
};
