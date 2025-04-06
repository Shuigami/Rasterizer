#include "object/sphere.hh"
#include "geometry/point.hh"
#include "geometry/vector.hh"

#include <cmath>

Sphere::Sphere(double radius, Point3 center, UniformTexture* texture)
: Object(texture), radius_(radius), center_(center)
{
}

bool Sphere::isInsideSphere(const Point3& p) const
{
    int dist = (p.getX() - center_.getX()) * (p.getX() - center_.getX()) +
        (p.getY() - center_.getY()) * (p.getY() - center_.getY()) +
        (p.getZ() - center_.getZ()) * (p.getZ() - center_.getZ());

    return dist < radius_ * radius_;
}

bool Sphere::isOnSphere(const Point3& p) const
{
    double dist = (p.getX() - center_.getX()) * (p.getX() - center_.getX()) +
        (p.getY() - center_.getY()) * (p.getY() - center_.getY()) +
        (p.getZ() - center_.getZ()) * (p.getZ() - center_.getZ());

    double threshold = 0.1;

    return dist < radius_ * radius_ + threshold && dist > radius_ * radius_ - threshold;
}

bool Sphere::intersect(const Point3& p, const Vector3& vect, double &t) const
{
    double a = 2 * (vect * (p - center_));

    Vector3 oMinusC = Vector3(p.getX() - center_.getX(), p.getY() - center_.getY(), p.getZ() - center_.getZ());

    double b = a * a - 4 * vect.getNorm() * vect.getNorm() * (oMinusC * oMinusC - radius_ * radius_);

    if (b < 0)
        return false;

    b = sqrt(b);

    double c = 2 * vect.getNorm() * vect.getNorm();

    double t1 = (-a + b) / c;
    double t2 = (-a - b) / c;

    if (t1 < 0 || t2 < 0)
        return false;
    else
        t = std::min(t1, t2);

    return true;
}

Vector3 Sphere::normal(const Point3& p) const
{
    if (!isOnSphere(p))
        throw std::invalid_argument("Point is not on the sphere");

    return Vector3(p.getX() - center_.getX(), p.getY() - center_.getY(), p.getZ() - center_.getZ());
}

double Sphere::getRadius() const
{
    return radius_;
}

const Point3& Sphere::getCenter() const
{
    return center_;
}
