#pragma once

#include "geometry/point.hh"
#include "image/color.hh"
class Light {
public:
    virtual const Color& getColor() const = 0;
    virtual double getIntensity() const = 0;
    virtual const Point3& getPosition() const = 0;
};
