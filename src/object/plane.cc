#include "object/plane.hh"
#include "geometry/vector.hh"
#include "object/object.hh"

Plane::Plane(Point3 p, Vector3 u, Vector3 v, UniformTexture* texture)
: Object(texture), p_(p), u_(u), v_(v)
{
}

bool Plane::intersect(const Point3& d, const Vector3& vect, double &t) const
{
    Vector3 n = normal(d);
    double denominateur = n * vect;

    if (std::abs(denominateur) < 0.001)
        return false;

    Vector3 w = (p_ - d);
    t = (n * w) / denominateur;

    if (t < 0.001)
        return false;

    return true;
}

Vector3 Plane::normal(const Point3&) const
{
    return u_.cross(v_).normalize();
}

const Point3& Plane::getP() const
{
    return p_;
}

const Vector3& Plane::getU() const
{
    return u_;
}

const Vector3& Plane::getV() const
{
    return v_;
}
