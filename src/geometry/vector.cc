#include "geometry/vector.hh"
#include "geometry/point.hh"

#include <cfloat>
#include <cmath>
#include <climits>
#include <iostream>

Vector3::Vector3(Point3 p)
: p_(p)
{
}

Vector3::Vector3(double x, double y, double z)
: p_(Point3(x, y, z))
{
}

const Point3& Vector3::getP() const
{
    return p_;
}

double Vector3::getAngle(const Vector3 &vect) const
{
    return acos((*this * vect) /(getNorm() * vect.getNorm()));
}

double Vector3::getNorm() const
{
    return sqrt(p_.getX() * p_.getX() + p_.getY() * p_.getY() + p_.getZ() * p_.getZ());
}

Vector3 Vector3::normalize()
{
    double norm = getNorm();

    if (std::abs(norm) < 0.001)
        return *this;

    p_ = p_ / norm;

    return *this;
}

Vector3 Vector3::normalize() const
{
    double norm = getNorm();

    if (std::abs(norm) < 0.001)
        return Vector3(p_ / 1);

    return Vector3(p_ / norm);
}

Vector3 Vector3::cross(const Vector3 &vect) const
{
    double x = p_.getY() * vect.getP().getZ() - p_.getZ() * vect.getP().getY();
    double y = p_.getZ() * vect.getP().getX() - p_.getX() * vect.getP().getZ();
    double z = p_.getX() * vect.getP().getY() - p_.getY() * vect.getP().getX();

    return Vector3(x, y, z);
}

Vector3 Vector3::operator+(const Vector3 &vect) const
{
    return Vector3(p_ + vect.getP());
}

Vector3 Vector3::operator-(const Vector3 &vect) const
{
    return Vector3(p_ - vect.getP());
}

Vector3 Vector3::operator*(const Matrix &m) const
{
    double x = m.get(0, 0) * p_.getX() + m.get(0, 1) * p_.getY() + m.get(0, 2) * p_.getZ();
    double y = m.get(1, 0) * p_.getX() + m.get(1, 1) * p_.getY() + m.get(1, 2) * p_.getZ();
    double z = m.get(2, 0) * p_.getX() + m.get(2, 1) * p_.getY() + m.get(2, 2) * p_.getZ();

    Point3 p(x, y, z);

    return Vector3(p);
}

Vector3 Vector3::operator*(const double &l) const
{
    return Vector3(p_ * l);
}

Vector3 Vector3::operator/(const double &l) const
{
    return Vector3(p_ / l);
}

double Vector3::operator*(const Vector3 &vect) const
{
    return p_.getX() * vect.getP().getX() + p_.getY() * vect.getP().getY() + p_.getZ() * vect.getP().getZ();
}

std::ostream& operator<<(std::ostream &out, const Vector3 &vect)
{
    return out << "Vector: " << vect.getP();
}

Vector4::Vector4(Vector3 v, double w)
: p_(Point4(v.getP(), w))
{
}

Vector4::Vector4(Point3 p, double w)
: p_(Point4(p, w))
{
}

Vector4::Vector4(Point4 p)
: p_(p)
{
}

Vector4::Vector4(double x, double y, double z, double w)
: p_(Point4(x, y, z, w))
{
}

const Point4& Vector4::getP() const
{
    return p_;
}

double Vector4::getAngle(const Vector4 &vect) const
{
    return acos((*this * vect) /(getNorm() * vect.getNorm()));
}

double Vector4::getNorm() const
{
    return sqrt(p_.getX() * p_.getX() + p_.getY() * p_.getY() + p_.getZ() * p_.getZ());
}

Vector4 Vector4::normalize()
{
    double norm = this->getP().getW();

    if (std::abs(norm) < 0.001)
        return *this;

    p_ = p_ / norm;

    return *this;
}

Vector4 Vector4::normalize() const
{
    double norm = this->getP().getW();

    if (std::abs(norm) < 0.001)
        return Vector4(p_ / 1);

    return Vector4(p_ / norm);
}

Vector4 Vector4::cross(const Vector4 &vect) const
{
    double x = p_.getY() * vect.getP().getZ() - p_.getZ() * vect.getP().getY();
    double y = p_.getZ() * vect.getP().getX() - p_.getX() * vect.getP().getZ();
    double z = p_.getX() * vect.getP().getY() - p_.getY() * vect.getP().getX();

    return Vector4(x, y, z, 1);
}

Vector4 Vector4::operator+(const Vector4 &vect) const
{
    return Vector4(p_ + vect.getP());
}

Vector4 Vector4::operator-(const Vector4 &vect) const
{
    return Vector4(p_ - vect.getP());
}

Vector4 Vector4::operator*(const Matrix &m) const
{
    double x = m.get(0, 0) * p_.getX() + m.get(0, 1) * p_.getY() + m.get(0, 2) * p_.getZ();
    double y = m.get(1, 0) * p_.getX() + m.get(1, 1) * p_.getY() + m.get(1, 2) * p_.getZ();
    double z = m.get(2, 0) * p_.getX() + m.get(2, 1) * p_.getY() + m.get(2, 2) * p_.getZ();

    Point4 p(x, y, z, 1);

    return Vector4(p);
}

Vector4 Vector4::operator*(const double &l) const
{
    return Vector4(p_ * l);
}

Vector4 Vector4::operator/(const double &l) const
{
    return Vector4(p_ / l);
}

double Vector4::operator*(const Vector4 &vect) const
{
    return p_.getX() * vect.getP().getX() + p_.getY() * vect.getP().getY() + p_.getZ() * vect.getP().getZ();
}

bool Vector4::operator==(const Vector4 &vect) const
{
    return getP() == vect.getP();
}

Vector3 Vector4::toVector3() const
{
    return Vector3(p_.getX(), p_.getY(), p_.getZ());
}

std::ostream& operator<<(std::ostream &out, const Vector4 &vect)
{
    return out << "Vector4: " << vect.getP();
}
