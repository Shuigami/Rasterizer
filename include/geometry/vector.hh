#pragma once

#include "geometry/matrix.hh"
#include "geometry/point.hh"

class Vector3 {
public:
    Vector3(Point3 p);
    Vector3(double x, double y, double z);

    const Point3& getP() const;

    double getAngle(const Vector3 &vect) const;
    double getNorm() const;

    Vector3 normalize();
    Vector3 normalize() const;

    Vector3 cross(const Vector3 &vect) const;

    Vector3 operator+(const Vector3 &vect) const;
    Vector3 operator-(const Vector3 &vect) const;
    Vector3 operator*(const Matrix &m) const;
    Vector3 operator*(const double &l) const;
    Vector3 operator/(const double &l) const;

    double operator*(const Vector3 &vect) const;

    friend std::ostream& operator<<(std::ostream &out, const Vector3 &vect);
private:
    Point3 p_;
};

class Vector4 {
public:
    Vector4(Vector3 v, double w = 1);
    Vector4(Point4 p);
    Vector4(Point3 p, double w = 1);
    Vector4(double x, double y, double z, double w = 1);

    const Point4& getP() const;

    double getAngle(const Vector4 &vect) const;
    double getNorm() const;

    Vector4 normalize();
    Vector4 normalize() const;

    Vector4 cross(const Vector4 &vect) const;

    Vector4 operator+(const Vector4 &vect) const;
    Vector4 operator-(const Vector4 &vect) const;
    Vector4 operator*(const Matrix &m) const;
    Vector4 operator*(const double &l) const;
    Vector4 operator/(const double &l) const;

    bool operator==(const Vector4 &vect) const;

    double operator*(const Vector4 &vect) const;

    Vector3 toVector3() const;

    friend std::ostream& operator<<(std::ostream &out, const Vector4 &vect);
private:
    Point4 p_;
};
