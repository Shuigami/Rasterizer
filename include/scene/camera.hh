#pragma once

#include "geometry/point.hh"
#include "geometry/vector.hh"

class Camera {
public:
    Camera(Point3 center, Point3 direction, Vector3 up, double alpha, double beta, double zmin);

    const Point3& getCenter() const;
    const Point3& getDirection() const;
    const Vector3& getUp() const;

    double getAlpha() const;
    double getBeta() const;
    double getZmin() const;

private:
    Point3 center_;
    Point3 direction_;
    Vector3 up_;
    double alpha_;
    double beta_;
    double zmin_;
};
