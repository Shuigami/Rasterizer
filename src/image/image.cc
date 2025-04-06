#include "image/image.hh"
#include "geometry/matrix.hh"
#include "image/color.hh"
#include "logger/logger.hh"

#include <fstream>

Image::Image(int height, int width, std::list<Color*> pixels)
: height_(height), width_(width), pixels_(pixels)
{
}

void Image::gradient_to_ppm(std::string path)
{
    Image img(height_, width_, std::list<Color*>());

    Color** pixels = new Color*[height_];
    for (int i = 0; i < height_; i++)
        pixels[i] = new Color[width_];

    int i = 0;
    int j = 0;

    for (auto p : pixels_)
    {
        pixels[i][j] = Color(0.21 * p->getR() + 0.72 * p->getG() + 0.07 * p->getB());
        i++;

        if (i == width_)
        {
            i = 0;
            j++;
        }
    }

    std::list<Color*> sx;
    std::list<Color*> sy;

    Matrix mx(3, 3);
    mx.set(0, 0, -1);
    mx.set(0, 1, 0);
    mx.set(0, 2, 1);
    mx.set(1, 0, -2);
    mx.set(1, 1, 0);
    mx.set(1, 2, 2);
    mx.set(2, 0, -1);
    mx.set(2, 1, 0);
    mx.set(2, 2, 1);

    Matrix my(3, 3);
    my.set(0, 0, -1);
    my.set(0, 1, -2);
    my.set(0, 2, -1);
    my.set(1, 0, 0);
    my.set(1, 1, 0);
    my.set(1, 2, 0);
    my.set(2, 0, 1);
    my.set(2, 1, 2);
    my.set(2, 2, 1);

    for (i = 0; i < width_; i++)
    {
        for (j = 0; j < height_; j++)
        {
            Color color = 0;
            for (int k = 0; k < 3; k++)
            {
                for (int l = 0; l < 3; l++)
                {
                    if (i + k - 1 < 0 || i + k - 1 >= width_ || j + l - 1 < 0 || j + l - 1 >= height_)
                        continue;

                    color = color + pixels[j + l - 1][i + k - 1] * mx.get(k, l);
                }
            }

            sx.push_back(new Color(color));

            color = 0;
            for (int k = 0; k < 3; k++)
            {
                for (int l = 0; l < 3; l++)
                {
                    if (i + k - 1 < 0 || i + k - 1 >= width_ || j + l - 1 < 0 || j + l - 1 >= height_)
                        continue;

                    color = color + pixels[j + l - 1][i + k - 1] * my.get(k, l);
                }
            }

            sy.push_back(new Color(color));
        }
    }

    Image img_sx(height_, width_, sx);
    img_sx.image_to_ppm("sx.ppm");

    Image img_sy(height_, width_, sy);
    img_sy.image_to_ppm("sy.ppm");

    (void) path;
}

void Image::image_to_ppm(std::string path)
{
    Logger::getInstance().log(Logger::Level::INFO, "Writing image to file...");

    std::ofstream myfile;
    myfile.open (path);

    myfile << "P3" << std::endl << std::endl
        << width_ << " " << height_ << std::endl << "255" << std::endl;

    int i = 0;
    for (auto p : pixels_)
    {
        myfile << p->getR() << " " << p->getG() << " " << p->getB() << " ";
        i++;

        if (i == width_)
        {
            myfile << std::endl;
            i = 0;
        }
    }

    myfile.close();

    Logger::getInstance().log(Logger::Level::INFO, "Image written to file.");
}
