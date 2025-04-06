#include "motor/motor.hh"
#include "geometry/point.hh"
#include "geometry/vector.hh"
#include "image/image.hh"
#include "scene/camera.hh"
#include "scene/scene.hh"
#include "logger/logger.hh"

#include <cmath>
#include <sys/types.h>

Motor::Motor(Scene scene)
: scene_(scene), width_(0), height_(0), ratio_(533.031282), width_pixels_(0), height_pixels_(0)
, forward(Vector3(0, 0, 0)), right(Vector3(0, 0, 0)), up(Vector3(0, 0, 0))
{
    width_ = 2 * scene_.getCamera().getZmin() * std::tan((scene_.getCamera().getAlpha() * M_PI / 180) / 2);
    height_ = 2 * scene_.getCamera().getZmin() * std::tan((scene_.getCamera().getBeta() * M_PI / 180) / 2);

    width_pixels_ = width_ * ratio_;
    height_pixels_ = height_ * ratio_;

    Logger::getInstance().log(Logger::Level::INFO, "Motor created.");
    Logger::getInstance().log(Logger::Level::INFO, "Width: " + std::to_string(width_) + " Height: " + std::to_string(height_));
    Logger::getInstance().log(Logger::Level::INFO, "Width pixels: " + std::to_string(width_pixels_) + " Height pixels: " + std::to_string(height_pixels_));

    const Camera &camera = scene_.getCamera();
    forward = Vector3(camera.getDirection() - camera.getCenter()).normalize();
    right = forward.cross(camera.getUp()).normalize();
    up = right.cross(forward).normalize();
}

Image Motor::render()
{
    // FIXME
    return Image(width_pixels_, height_pixels_, std::list<Color *>());
}
