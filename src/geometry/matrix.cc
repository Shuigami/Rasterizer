#include "geometry/matrix.hh"

#include <cmath>
#include <iostream>

Matrix::Matrix(int n, int m)
{
    n_ = n;
    m_ = m;

    matrix_ = new double*[n_];
    for (int i = 0; i < n_; i++)
        matrix_[i] = new double[m_];
}

Matrix::Matrix(double **matrix, int n, int m)
{
    n_ = n;
    m_ = m;

    matrix_ = new double*[n_];
    for (int i = 0; i < n_; i++)
        matrix_[i] = new double[m_];

    for (int i = 0; i < n_; i++)
        for (int j = 0; j < m_; j++)
            matrix_[i][j] = matrix[i][j];
}

Matrix::Matrix(const Matrix &matrix)
{
    n_ = matrix.n_;
    m_ = matrix.m_;

    matrix_ = new double*[n_];
    for (int i = 0; i < n_; i++)
        matrix_[i] = new double[m_];

    for (int i = 0; i < n_; i++)
        for (int j = 0; j < m_; j++)
            matrix_[i][j] = matrix.matrix_[i][j];
}

Matrix::~Matrix()
{
    for (int i = 0; i < n_; i++)
        delete[] matrix_[i];
    delete[] matrix_;
}

double Matrix::get(int i, int j) const
{
    return matrix_[i][j];
}

void Matrix::set(int i, int j, double val)
{
    matrix_[i][j] = val;
}

Matrix Matrix::inverse() const
{
    Matrix res(n_, m_);

    Matrix matrix(*this);

    for (int i = 0; i < n_; i++)
        res.set(i, i, 1);

    for (int i = 0; i < n_; i++) {
        double pivot = matrix.get(i, i);
        if (pivot == 0) return Matrix(0, 0);

        for (int j = 0; j < n_; j++) {
            matrix.set(i, j, matrix.get(i, j) / pivot);
            res.set(i, j, res.get(i, j) / pivot);
        }

        for (int k = 0; k < n_; k++) {
            if (k != i) {
                double factor = matrix.get(k, i);
                for (int j = 0; j < n_; j++) {
                    matrix.set(k, j, matrix.get(k, j) - factor * matrix.get(i, j));
                    res.set(k, j, res.get(k, j) - factor * res.get(i, j));
                }
            }
        }
    }

    return res;
}

Matrix Matrix::operator+(const Matrix &matrix) const
{
    Matrix res(n_, m_);

    for (int i = 0; i < n_; i++)
        for (int j = 0; j < m_; j++)
            res.set(i, j, matrix_[i][j] + matrix.matrix_[i][j]);

    return res;
}

Matrix Matrix::operator-(const Matrix &matrix) const
{
    Matrix res(n_, m_);

    for (int i = 0; i < n_; i++)
        for (int j = 0; j < m_; j++)
            res.set(i, j, matrix_[i][j] - matrix.matrix_[i][j]);

    return res;
}

Matrix Matrix::operator*(const Matrix &matrix) const
{
    Matrix res(n_, matrix.m_);

    for (int i = 0; i < n_; i++)
        for (int j = 0; j < matrix.m_; j++)
        {
            double sum = 0;
            for (int k = 0; k < m_; k++)
                sum += matrix_[i][k] * matrix.matrix_[k][j];
            res.set(i, j, sum);
        }

    return res;
}

Matrix Matrix::operator*(double l) const
{
    Matrix res(n_, m_);

    for (int i = 0; i < n_; i++)
        for (int j = 0; j < m_; j++)
            res.set(i, j, matrix_[i][j] * l);

    return res;
}

Matrix Matrix::operator/(double l) const
{
    Matrix res(n_, m_);

    for (int i = 0; i < n_; i++)
        for (int j = 0; j < m_; j++)
            res.set(i, j, matrix_[i][j] / l);

    return res;
}

std::ostream& operator<<(std::ostream &out, const Matrix &matrix)
{
    out << "Matrix:" << std::endl;
    out << "n: " << matrix.n_ << std::endl;
    out << "m: " << matrix.m_ << std::endl;

    for (int i = 0; i < matrix.n_; i++)
    {
        for (int j = 0; j < matrix.m_; j++)
            out << matrix.get(i, j) << " ";
        out << std::endl;
    }

    return out;
}

Matrix Matrix::rotX(double n)
{
    double **rotX = new double*[3];
    for (int i = 0; i < 3; i++)
        rotX[i] = new double[3];

    rotX[0][0] = 1;
    rotX[0][1] = 0;
    rotX[0][2] = 0;
    rotX[1][0] = 0;
    rotX[1][1] = cos(n);
    rotX[1][2] = -sin(n);
    rotX[2][0] = 0;
    rotX[2][1] = sin(n);
    rotX[2][2] = cos(n);

    Matrix matRotX(rotX, 3, 3);

    for (int i = 0; i < 3; i++)
        delete[] rotX[i];
    delete[] rotX;

    return matRotX;
}

Matrix Matrix::rotY(double n)
{
    double **rotY = new double*[3];
    for (int i = 0; i < 3; i++)
        rotY[i] = new double[3];

    rotY[0][0] = cos(n);
    rotY[0][1] = 0;
    rotY[0][2] = sin(n);
    rotY[1][0] = 0;
    rotY[1][1] = 1;
    rotY[1][2] = 0;
    rotY[2][0] = -sin(n);
    rotY[2][1] = 0;
    rotY[2][2] = cos(n);

    Matrix matRotY(rotY, 3, 3);

    for (int i = 0; i < 3; i++)
        delete[] rotY[i];
    delete[] rotY;

    return matRotY;
}

Matrix Matrix::rotZ(double n)
{
    double **rotZ = new double*[3];
    for (int i = 0; i < 3; i++)
        rotZ[i] = new double[3];

    rotZ[0][0] = cos(n);
    rotZ[0][1] = -sin(n);
    rotZ[0][2] = 0;
    rotZ[1][0] = sin(n);
    rotZ[1][1] = cos(n);
    rotZ[1][2] = 0;
    rotZ[2][0] = 0;
    rotZ[2][1] = 0;
    rotZ[2][2] = 1;

    Matrix matRotZ(rotZ, 3, 3);

    for (int i = 0; i < 3; i++)
        delete[] rotZ[i];
    delete[] rotZ;

    return matRotZ;
}
