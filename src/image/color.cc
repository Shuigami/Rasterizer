#include "image/color.hh"

Color::Color(int r, int g, int b)
: r_(r > 255 ? 255 : r < 0 ? 0 : r), g_(g > 255 ? 255 : g < 0 ? 0 : g), b_(b > 255 ? 255 : b < 0 ? 0 : b)
{
}

Color::Color(int c)
: r_(c > 255 ? 255 : c < 0 ? 0 : c), g_(c > 255 ? 255 : c < 0 ? 0 : c), b_(c > 255 ? 255 : c < 0 ? 0 : c)
{
}

Color::Color(const Color &color)
: r_(color.r_), g_(color.g_),  b_(color.b_)
{
}

Color::Color()
{}

int Color::getR() const
{
    return r_;
}

int Color::getG() const
{
    return g_;
}

int Color::getB() const
{
    return b_;
}

Color Color::operator*(double n) const
{
    return Color(r_ * n, g_ * n, b_ * n);
}

Color Color::operator+(const Color &color) const
{
    return Color(r_ + color.r_, g_ + color.g_, b_ + color.b_);
}

Color Color::operator/(double n) const
{
    return Color(r_ / n, g_ / n, b_ / n);
}

Color Color::operator=(const Color &color)
{
    r_ = color.r_;
    g_ = color.g_;
    b_ = color.b_;

    return *this;
}

std::ostream& operator<<(std::ostream &out, const Color &color)
{
    out << color.getR() << " " << color.getG() << " " << color.getB();
    return out;
}
