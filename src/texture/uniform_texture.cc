#include "texture/uniform_texture.hh"
#include "texture/texture_material.hh"

UniformTexture::UniformTexture(Color color, double ka, double kd, double ks, double kr, double ns)
: TextureMaterial(color), ka_(ka), kd_(kd), ks_(ks), kr_(kr), ns_(ns)
{
}

std::map<std::string, double> UniformTexture::getTextureInfo() const
{
    std::map<std::string, double> info;

    info["ka"] = ka_;
    info["kd"] = kd_;
    info["ks"] = ks_;
    info["kr"] = kr_;
    info["ns"] = ns_;

    return info;
}

UniformTexture* UniformTexture::Plastic(Color color)
{
    return new UniformTexture(color, 0.2, 0.6, 0.3, 0.1, 50);
}

UniformTexture* UniformTexture::PlasticMat(Color color)
{
    return new UniformTexture(color, 0.2, 0.8, 0, 0, 10);
}

UniformTexture* UniformTexture::Metal(Color color)
{
    return new UniformTexture(color, 0.1, 0.3, 0.6, 0.7, 100);
}
