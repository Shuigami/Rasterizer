#include "light/point_light.hh"
#include "image/color.hh"

PointLight::PointLight(Color color, Point3 position, double intensity)
: color_(color), position_(position), intensity_(intensity)
{
}

const Color& PointLight::getColor() const
{
    return color_;
}

double PointLight::getIntensity() const
{
    return intensity_;
}

const Point3& PointLight::getPosition() const
{
    return position_;
}
