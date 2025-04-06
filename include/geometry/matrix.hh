#pragma once

#include <ostream>

class Matrix {
public:
    Matrix(int n, int m);
    Matrix(double **matrix, int n, int m);
    Matrix(const Matrix &m);
    ~Matrix();

    double get(int i, int j) const;
    void set(int i, int j, double val);

    Matrix inverse() const;

    Matrix operator+(const Matrix &m) const;
    Matrix operator-(const Matrix &m) const;
    Matrix operator*(const Matrix &m) const;
    Matrix operator*(double l) const;
    Matrix operator/(double l) const;

    friend std::ostream& operator<<(std::ostream &out, const Matrix &m);

    static Matrix rotX(double n);
    static Matrix rotY(double n);
    static Matrix rotZ(double n);
private:
    int n_;
    int m_;
    double **matrix_;
};
