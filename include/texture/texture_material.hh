#pragma once

#include "image/color.hh"
#include <string>
#include <map>

class TextureMaterial {
public:
    TextureMaterial(Color color);
    virtual std::map<std::string, double> getTextureInfo() const = 0;
    Color getColor() const;
private:
    Color color_;
};
