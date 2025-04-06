#include "scene/camera.hh"

Camera::Camera(Point3 center, Point3 direction, Vector3 up, double alpha, double beta, double zmin)
: center_(center), direction_(direction), up_(up), alpha_(alpha), beta_(beta), zmin_(zmin)
{
}

const Point3& Camera::getCenter() const
{
    return center_;
}

const Point3& Camera::getDirection() const
{
    return direction_;
}

const Vector3& Camera::getUp() const
{
    return up_;
}

double Camera::getAlpha() const
{
    return alpha_;
}

double Camera::getBeta() const
{
    return beta_;
}

double Camera::getZmin() const
{
    return zmin_;
}
