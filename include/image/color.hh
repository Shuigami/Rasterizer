#pragma once

#include <ostream>
class Color {
public:
    Color(int r, int g, int b);
    Color(int c);
    Color(const Color &color);
    Color();

    int getR() const;
    int getG() const;
    int getB() const;

    Color operator*(double n) const;
    Color operator+(const Color &color) const;
    Color operator/(double n) const;
    Color operator=(const Color &color);

    friend std::ostream& operator<<(std::ostream &out, const Color &color);
private:
    int r_;
    int g_;
    int b_;
};
