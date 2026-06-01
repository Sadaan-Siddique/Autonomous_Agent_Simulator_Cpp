#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include "../core/vector2d.hpp"

class Matrix {
private:
    int rows;
    int cols;
    std::vector<std::vector<double>> data;

public:
    // The Golden Rule of C++ Architecture

    // The "Heavier" class should carry the burden of translation. Since Matrix already knows about Vector2D, it is perfectly safe for Matrix to handle both sides of the translation:

    // Vector -> Matrix: Handled via the Matrix(const Vector2D&) constructor inside Matrix.
    // Matrix -> Vector: Handled via the operator Vector2D() const conversion operator inside Matrix.

    Matrix(int r, int c);
    Matrix(const Vector2D&);

    void setValue(int r, int c, double value);

    // Arithmetic operators (Marked const because they don't modify the original matrix)
    Matrix operator+(const Matrix& other) const;
    Matrix operator*(double scalar) const;
    Matrix operator*(const Matrix& other) const;
    Vector2D operator*(const Vector2D& vec) const;

    explicit operator Vector2D() const; //  In C++, the explicit keyword prevents the compiler from performing unintended implicit type conversions and copy-initializations. It is primarily applied to constructors and user-defined conversion operators to ensure that type changes are intentionally written by the programmer rather than assumed by the language.
    
    Matrix transpose() const;
    void display() const;
};

#endif