#pragma once

#include <string>
#include <vector>
#include "vector.h"

class Texture {
public:
    Texture();
    ~Texture();

    bool loadFromFile(const std::string& filename);
    bool create(int width, int height);

    Color sample(float u, float v) const;
    Color getPixel(int x, int y) const;
    void setPixel(int x, int y, const Color& color);

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    int m_width;
    int m_height;
    std::vector<Color> m_pixels;

    // Utility functions for texture sampling
    Color bilinearSample(float u, float v) const;
    int clamp(int value, int min, int max) const;
    float clamp(float value, float min, float max) const;
};
