#ifndef MATRIX_H
#define MATRIX_H

#include <vector>

class Matrix {
private:
    int rows;
    int cols;
    std::vector<std::vector<double>> data;

public:
    Matrix(int r, int c);

    void setValue(int r, int c, double value);

    // Arithmetic operators (Marked const because they don't modify the original matrix)
    Matrix operator+(const Matrix& other) const;
    Matrix operator*(double scalar) const;
    Matrix operator*(const Matrix& other) const;
    
    Matrix transpose() const;
    void display() const;
};

#endif