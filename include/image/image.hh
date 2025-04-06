#pragma once

#include "color.hh"
#include <list>
#include <string>

class Image {
public:
    Image(int height, int width, std::list<Color*> pixels);

    void gradient_to_ppm(std::string path);

    void image_to_ppm(std::string path);

private:
    int height_;
    int width_;
    std::list<Color*> pixels_;
};
