#include "geometry/point.hh"
#include <cmath>
#include <iostream>

Point3::Point3(double x, double y, double z)
: x_(x), y_(y), z_(z)
{
}

Point3::Point3(const Point3& p)
: x_(p.getX()), y_(p.getY()), z_(p.getZ())
{
}

double Point3::getX() const
{
    return x_;
}

double Point3::getY() const
{
    return y_;
}

double Point3::getZ() const
{
    return z_;
}

Point3 Point3::operator+(const Point3 &p) const
{
    return Point3(x_ + p.getX(), y_ + p.getY(), z_ + p.getZ());
}

Point3 Point3::operator-(const Point3 &p) const
{
    return Point3(x_ - p.getX(), y_ - p.getY(), z_ - p.getZ());
}

Point3 Point3::operator*(const double &l) const
{
    return Point3(x_ * l, y_ * l, z_ * l);
}

Point3 Point3::operator*(const Matrix &m) const
{
    return Point3(m.get(0, 0) * x_ + m.get(0, 1) * y_ + m.get(0, 2) * z_,
                  m.get(1, 0) * x_ + m.get(1, 1) * y_ + m.get(1, 2) * z_,
                  m.get(2, 0) * x_ + m.get(2, 1) * y_ + m.get(2, 2) * z_);
}

Point3 Point3::operator/(const double &l) const
{
    return Point3(x_ / l, y_ / l, z_ / l);
}

Point3 Point3::operator=(const Point3 &p)
{
    x_ = p.getX();
    y_ = p.getY();
    z_ = p.getZ();

    return *this;
}

std::ostream& operator<<(std::ostream &out, const Point3 &p)
{
    return out << "Point: (" << p.getX() << "," << p.getY() << "," << p.getZ() << ")";
}

Point4::Point4(double x, double y, double z, double w)
: x_(x), y_(y), z_(z), w_(w)
{
}

Point4::Point4(const Point3& p, double w)
: x_(p.getX()), y_(p.getY()), z_(p.getZ()), w_(w)
{
}

Point4::Point4(const Point4& p)
: x_(p.getX()), y_(p.getY()), z_(p.getZ()), w_(p.getW())
{
}

double Point4::getX() const
{
    return x_;
}

double Point4::getY() const
{
    return y_;
}

double Point4::getZ() const
{
    return z_;
}

double Point4::getW() const
{
    return w_;
}

Point4 Point4::operator+(const Point4 &p) const
{
    return Point4(x_ + p.getX(), y_ + p.getY(), z_ + p.getZ(), w_ + p.getW());
}

Point4 Point4::operator-(const Point4 &p) const
{
    return Point4(x_ - p.getX(), y_ - p.getY(), z_ - p.getZ(), w_ - p.getW());
}

Point4 Point4::operator*(const double &l) const
{
    return Point4(x_ * l, y_ * l, z_ * l, w_ * l);
}

Point4 Point4::operator*(const Matrix &m) const
{
    return Point4(m.get(0, 0) * x_ + m.get(0, 1) * y_ + m.get(0, 2) * z_ + m.get(0, 3) * w_,
                  m.get(1, 0) * x_ + m.get(1, 1) * y_ + m.get(1, 2) * z_ + m.get(1, 3) * w_,
                  m.get(2, 0) * x_ + m.get(2, 1) * y_ + m.get(2, 2) * z_ + m.get(2, 3) * w_,
                  m.get(3, 0) * x_ + m.get(3, 1) * y_ + m.get(3, 2) * z_ + m.get(3, 3) * w_);
}

Point4 Point4::operator/(const double &l) const
{
    return Point4(x_ / l, y_ / l, z_ / l, w_ / l);
}

Point4 Point4::operator=(const Point4 &p)
{
    x_ = p.getX();
    y_ = p.getY();
    z_ = p.getZ();
    w_ = p.getW();

    return *this;
}

bool Point4::operator==(const Point4 &p) const
{
    return std::fmod(p.getX(), x_) == std::fmod(p.getY(), y_) && std::fmod(p.getX(), x_) == std::fmod(p.getZ(), z_);
}

Point3 Point4::toPoint3() const
{
    return Point3(x_, y_, z_);
}

std::ostream& operator<<(std::ostream &out, const Point4 &p)
{
    return out << "Point: (" << p.getX() << "," << p.getY() << "," << p.getZ() << "," << p.getW() << ")";
}
