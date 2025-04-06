#pragma once

#include "image/image.hh"
#include "scene/scene.hh"

class Motor {
public:
    Motor(Scene scene);

    Image render();

private:
    Scene scene_;
    double width_;
    double height_;
    double ratio_;
    int width_pixels_;
    int height_pixels_;

    Vector3 forward;
    Vector3 right;
    Vector3 up;
};
