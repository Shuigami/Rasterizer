#pragma once

#include "geometry/point.hh"
#include "geometry/vector.hh"
#include "object/object.hh"
#include "texture/uniform_texture.hh"

class Plane : public Object {
public:
    Plane(Point3 p, Vector3 u, Vector3 v, UniformTexture* texture);

    bool intersect(const Point3& p, const Vector3& vect, double &t) const override;
    Vector3 normal(const Point3& p) const override;

    const Point3& getP() const;
    const Vector3& getU() const;
    const Vector3& getV() const;

private:
    Point3 p_;
    Vector3 u_;
    Vector3 v_;
};
