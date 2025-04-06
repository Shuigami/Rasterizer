#pragma once

#include "object/object.hh"
#include "texture/texture_material.hh"

class Triangle : public Object {
    public:
        Triangle(Point3 p1, Point3 p2, Point3 p3, TextureMaterial* texture);

        bool intersect(const Point3& p, const Vector3& vect, double &t) const override;
        Vector3 normal(const Point3& p) const override;

        const Point3& getP1() const;
        const Point3& getP2() const;
        const Point3& getP3() const;

    private:
        Point3 p1_;
        Point3 p2_;
        Point3 p3_;
};
