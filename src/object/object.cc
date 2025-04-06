#include "object/object.hh"

Object::Object(TextureMaterial* texture)
: texture_(texture)
{
}

std::map<std::string, double> Object::getTextureInfo() const
{
    return texture_->getTextureInfo();
}

Color Object::getColor()
{
    return texture_->getColor();
}
