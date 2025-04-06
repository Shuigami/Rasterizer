#include "texture/texture_material.hh"

TextureMaterial::TextureMaterial(Color color)
: color_(color)
{
}

Color TextureMaterial::getColor() const
{
    return color_;
}
