#pragma once

#include "geometry/point.hh"
#include "object/object.hh"
#include "texture/uniform_texture.hh"

class Sphere : public Object {
public:
    Sphere(double radius, Point3 center, UniformTexture* texture);

    bool isInsideSphere(const Point3& p) const;
    bool isOnSphere(const Point3& p) const;

    bool intersect(const Point3& p, const Vector3& vect, double &t) const override;
    Vector3 normal(const Point3& p) const override;

    double getRadius() const;
    const Point3& getCenter() const;

private:
    double radius_;
    Point3 center_;
};
