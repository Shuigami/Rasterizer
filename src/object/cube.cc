#include "object/cube.hh"

#include "object/plane.hh"
#include <limits>

Cube::Cube(Point3 p, double size, TextureMaterial* texture) : Object(texture), p_(p), size_(size), planes() {
    planes.push_back(Plane(p, Vector3(1, 0, 0), Vector3(0, 1, 0), nullptr));
    planes.push_back(Plane(p, Vector3(1, 0, 0), Vector3(0, 0, 1), nullptr));
    planes.push_back(Plane(p, Vector3(0, 1, 0), Vector3(0, 0, 1), nullptr));
    planes.push_back(Plane(p + Point3(0, 0, size), Vector3(1, 0, 0), Vector3(0, 1, 0), nullptr));
    planes.push_back(Plane(p + Point3(size, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1), nullptr));
    planes.push_back(Plane(p + Point3(0, size, 0), Vector3(1, 0, 0), Vector3(0, 0, 1), nullptr));
}

bool Cube::intersect(const Point3& p, const Vector3& vect, double &t) const {

    double min_t = std::numeric_limits<double>::infinity();

    for (const Plane& plane : planes) {
        double t_plane;
        if (plane.intersect(p, vect, t_plane)) {
            if (t_plane < min_t) {
                Point3 intersection = p + vect.getP() * t_plane;

                if (intersection.getX() < p_.getX() || intersection.getX() > p_.getX() + size_ ||
                    intersection.getY() < p_.getY() || intersection.getY() > p_.getY() + size_ ||
                    intersection.getZ() < p_.getZ() || intersection.getZ() > p_.getZ() + size_) {
                    continue;
                }

                min_t = t_plane;
            }
        }
    }

    if (min_t == std::numeric_limits<double>::infinity()) {
        return false;
    }

    t = min_t;
    return true;
}

Vector3 Cube::normal(const Point3& p) const {
    double t = 0;
    for (const Plane& plane : planes) {
        if (plane.intersect(p, Vector3(1, 0, 0), t)) {
            return plane.normal(p);
        }

        if (plane.intersect(p, Vector3(0, 1, 0), t)) {
            return plane.normal(p);
        }

        if (plane.intersect(p, Vector3(0, 0, 1), t)) {
            return plane.normal(p);
        }
    }

    return Vector3(0, 0, 0);
}

const Point3& Cube::getP() const {
    return p_;
}

double Cube::getSize() const {
    return size_;
}
