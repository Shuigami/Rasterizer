#pragma once

#include "geometry/point.hh"
#include "light/light.hh"

class PointLight : public Light {
public:
    PointLight(Color color, Point3 position, double intensity);

    const Color& getColor() const override;
    double getIntensity() const override;
    const Point3& getPosition() const override;

private:
    Color color_;
    Point3 position_;
    double intensity_;
};
