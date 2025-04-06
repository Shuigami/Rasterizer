#pragma once

#include "image/color.hh"
#include "texture/texture_material.hh"

class UniformTexture : public TextureMaterial {
public:
    UniformTexture(Color color, double ka, double kd, double ks, double kr, double ns);
    std::map<std::string, double> getTextureInfo() const override;

    static UniformTexture* Plastic(Color color);
    static UniformTexture* PlasticMat(Color color);
    static UniformTexture* Metal(Color color);

private:
    double ka_;
    double kd_;
    double ks_;
    double kr_;
    double ns_;
};
