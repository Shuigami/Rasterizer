#pragma once

#include "geometry/matrix.hh"
#include <ostream>

class Point3 {
public:
    Point3(double x, double y, double z);
    Point3(const Point3& p);

    double getX() const;
    double getY() const;
    double getZ() const;

    Point3 operator+(const Point3 &p) const;
    Point3 operator-(const Point3 &p) const;
    Point3 operator*(const Matrix &m) const;
    Point3 operator*(const double &l) const;
    Point3 operator/(const double &l) const;

    Point3 operator=(const Point3 &p);

    friend std::ostream& operator<<(std::ostream &out, const Point3 &p);
private:
    double x_;
    double y_;
    double z_;
};

class Point4 {
public:
    Point4(double x, double y, double z, double w = 1);
    Point4(const Point3& p, double w = 1);
    Point4(const Point4& p);

    double getX() const;
    double getY() const;
    double getZ() const;
    double getW() const;

    Point4 operator+(const Point4 &p) const;
    Point4 operator-(const Point4 &p) const;
    Point4 operator*(const Matrix &m) const;
    Point4 operator*(const double &l) const;
    Point4 operator/(const double &l) const;

    Point4 operator=(const Point4 &p);

    bool operator==(const Point4 &p) const;

    Point3 toPoint3() const;

    friend std::ostream& operator<<(std::ostream &out, const Point4 &p);
private:
    double x_;
    double y_;
    double z_;
    double w_;
};
