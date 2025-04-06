#pragma once

#include "object/object.hh"
#include "object/plane.hh"
#include "texture/texture_material.hh"
#include <list>

class Cube : public Object {
    public:
        Cube(Point3 p, double size, TextureMaterial* texture);

        bool intersect(const Point3& p, const Vector3& vect, double &t) const override;
        Vector3 normal(const Point3& p) const override;

        const Point3& getP() const;
        double getSize() const;
    private:
        Point3 p_;
        double size_;
        std::list<Plane> planes;
};
