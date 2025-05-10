#include "texture.h"
#include <SDL_image.h>
#include <iostream>
#include <algorithm>

Texture::Texture() : m_width(0), m_height(0) {
    // Initialize SDL_image if not already initialized
    static bool initialized = false;
    if (!initialized) {
        int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
            std::cerr << "SDL_image could not initialize! SDL_image Error: "
                      << IMG_GetError() << std::endl;
        }
        initialized = true;
    }
}

Texture::~Texture() {
    // No need to manually free m_pixels as std::vector handles its own memory
}

bool Texture::loadFromFile(const std::string& filename) {
    // Load image from file
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface) {
        std::cerr << "Unable to load image " << filename << "! SDL_image Error: "
                  << IMG_GetError() << std::endl;
        return false;
    }

    // Store dimensions
    m_width = surface->w;
    m_height = surface->h;

    // Convert surface to RGBA format if it's not already
    SDL_Surface* rgbaSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    if (!rgbaSurface) {
        std::cerr << "Unable to convert surface to RGBA! SDL Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return false;
    }

    // Free the original surface if we converted
    if (surface != rgbaSurface) {
        SDL_FreeSurface(surface);
        surface = rgbaSurface;
    }

    // Resize our pixel vector
    m_pixels.resize(m_width * m_height);

    // Copy pixel data
    uint32_t* pixels = static_cast<uint32_t*>(surface->pixels);
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            uint32_t pixel = pixels[y * surface->pitch / 4 + x];

            // Extract RGBA components
            uint8_t r = (pixel >> 0) & 0xFF;
            uint8_t g = (pixel >> 8) & 0xFF;
            uint8_t b = (pixel >> 16) & 0xFF;
            uint8_t a = (pixel >> 24) & 0xFF;

            // Store in our texture
            m_pixels[y * m_width + x] = Color(r, g, b, a);
        }
    }

    // Free the SDL surface
    SDL_FreeSurface(surface);

    return true;
}

bool Texture::create(int width, int height) {
    // Check for valid dimensions
    if (width <= 0 || height <= 0) {
        std::cerr << "Invalid texture dimensions: " << width << "x" << height << std::endl;
        return false;
    }

    // Store dimensions
    m_width = width;
    m_height = height;

    // Resize and initialize with transparent black
    m_pixels.resize(width * height, Color(0, 0, 0, 0));

    return true;
}

Color Texture::sample(float u, float v) const {
    // If texture is empty, return transparent black
    if (m_width == 0 || m_height == 0) {
        return Color(0, 0, 0, 0);
    }

    // Use bilinear sampling for better quality
    return bilinearSample(u, v);
}

Color Texture::getPixel(int x, int y) const {
    // If texture is empty or coordinates are out of bounds, return transparent black
    if (m_width == 0 || m_height == 0 || x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return Color(0, 0, 0, 0);
    }

    // Return the pixel color
    return m_pixels[y * m_width + x];
}

void Texture::setPixel(int x, int y, const Color& color) {
    // If texture is empty or coordinates are out of bounds, do nothing
    if (m_width == 0 || m_height == 0 || x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return;
    }

    // Set the pixel color
    m_pixels[y * m_width + x] = color;
}

Color Texture::bilinearSample(float u, float v) const {
    // Handle texture wrapping (repeat mode)
    u = u - std::floor(u);
    v = v - std::floor(v);

    // Convert to texture coordinates
    float x = u * m_width - 0.5f;  // Texel centers are at (0.5, 0.5), (1.5, 0.5), etc.
    float y = v * m_height - 0.5f;

    // Get the four neighboring texels
    int x0 = static_cast<int>(std::floor(x));
    int y0 = static_cast<int>(std::floor(y));
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // Calculate the fractional components
    float fx = x - x0;
    float fy = y - y0;

    // Get the four texel colors with wrapping
    Color c00 = getPixel(clamp(x0, 0, m_width - 1), clamp(y0, 0, m_height - 1));
    Color c10 = getPixel(clamp(x1, 0, m_width - 1), clamp(y0, 0, m_height - 1));
    Color c01 = getPixel(clamp(x0, 0, m_width - 1), clamp(y1, 0, m_height - 1));
    Color c11 = getPixel(clamp(x1, 0, m_width - 1), clamp(y1, 0, m_height - 1));

    // Bilinear interpolation for each color component
    float r = (1.0f - fx) * (1.0f - fy) * c00.r +
              fx * (1.0f - fy) * c10.r +
              (1.0f - fx) * fy * c01.r +
              fx * fy * c11.r;

    float g = (1.0f - fx) * (1.0f - fy) * c00.g +
              fx * (1.0f - fy) * c10.g +
              (1.0f - fx) * fy * c01.g +
              fx * fy * c11.g;

    float b = (1.0f - fx) * (1.0f - fy) * c00.b +
              fx * (1.0f - fy) * c10.b +
              (1.0f - fx) * fy * c01.b +
              fx * fy * c11.b;

    float a = (1.0f - fx) * (1.0f - fy) * c00.a +
              fx * (1.0f - fy) * c10.a +
              (1.0f - fx) * fy * c01.a +
              fx * fy * c11.a;

    return Color(
        static_cast<uint8_t>(std::clamp(r, 0.0f, 255.0f)),
        static_cast<uint8_t>(std::clamp(g, 0.0f, 255.0f)),
        static_cast<uint8_t>(std::clamp(b, 0.0f, 255.0f)),
        static_cast<uint8_t>(std::clamp(a, 0.0f, 255.0f))
    );
}

int Texture::clamp(int value, int min, int max) const {
    return std::clamp(value, min, max);
}

float Texture::clamp(float value, float min, float max) const {
    return std::clamp(value, min, max);
}
