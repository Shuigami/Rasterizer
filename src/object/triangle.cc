#include "object/triangle.hh"
#include "geometry/vector.hh"

Triangle::Triangle(Point3 p1, Point3 p2, Point3 p3, TextureMaterial* texture)
: Object(texture), p1_(p1), p2_(p2), p3_(p3)
{
}

bool Triangle::intersect(const Point3& p, const Vector3& vect, double &t) const
{
    Vector3 n1 = Vector3(p1_ - p).cross(Vector3(p2_ - p));
    Vector3 n2 = Vector3(p2_ - p).cross(Vector3(p3_ - p));
    Vector3 n3 = Vector3(p3_ - p).cross(Vector3(p1_ - p));

    double pn1 = vect * n1;
    double pn2 = vect * n2;
    double pn3 = vect * n3;

    if (!(pn1 >= 0 && pn2 >= 0 && pn3 >= 0) && !(pn1 <= 0 && pn2 <= 0 && pn3 <= 0))
        return false;

    Vector3 n = n1 + n2 + n3;
    double a = vect * n;

    if (a > -0.00001 && a < 0.00001)
        return false;

    Vector3 s = Vector3(p1_ - p);
    double b = s * n;

    t = b / a;
    return true;
}

Vector3 Triangle::normal(const Point3& p) const
{
    (void) p;

    Vector3 e1 = Vector3(p2_ - p1_);
    Vector3 e2 = Vector3(p3_ - p1_);

    return (e1.cross(e2)).normalize();
}

const Point3& Triangle::getP1() const
{
    return p1_;
}

const Point3& Triangle::getP2() const
{
    return p2_;
}

const Point3& Triangle::getP3() const
{
    return p3_;
}
