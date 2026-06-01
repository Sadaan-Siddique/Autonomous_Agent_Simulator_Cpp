#include "../../include/math/matrix.hpp"
#include <iostream>

Matrix::Matrix(int r, int c) : rows(r), cols(c)
{
    data.resize(rows, std::vector<double>(cols, 0.0));
}

void Matrix::setValue(int r, int c, double value)
{
    if (r < rows && c < cols)
    {
        data[r][c] = value;
    }
}

Matrix::Matrix(const Vector2D &vec) 
{
    rows = 2;
    cols = 1;
    
    // MUST allocate the memory first!
    data.resize(rows, std::vector<double>(cols, 0.0));
    
    data[0][0] = vec.m_x; // X goes in Row 0, Col 0
    data[1][0] = vec.m_y; // Y goes in Row 1, Col 0
}

Matrix::operator Vector2D() const
{
    // Safety check: We can only make a Vector2D out of a 2x1 Matrix
    if (rows == 2 && cols == 1) 
    {
        // Extract X from row 0, Y from row 1
        return Vector2D(data[0][0], data[1][0]); 
    }
    
    std::cout << "Error: Cannot cast a " << rows << "x" << cols << " matrix to a Vector2D.\n";
    return Vector2D(0.0f, 0.0f); // Safe fallback
}

Matrix Matrix::operator+(const Matrix &other) const
{
    Matrix result(rows, cols);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            result.data[i][j] = this->data[i][j] + other.data[i][j];
        }
    }
    return result;
}

Matrix Matrix::operator*(double scalar) const
{
    Matrix result(rows, cols);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            result.data[i][j] = this->data[i][j] * scalar;
        }
    }
    return result;
}

Matrix Matrix::operator*(const Matrix &other) const
{
    Matrix result(this->rows, other.cols);
    for (int i = 0; i < this->rows; i++)
    {
        for (int j = 0; j < other.cols; j++)
        {
            for (int k = 0; k < this->cols; k++)
            {
                result.data[i][j] += this->data[i][k] * other.data[k][j];
            }
        }
    }
    return result;
}

// Vector2D Matrix::operator*(const Vector2D &vec) const
// {
//     Matrix vecMatrix = static_cast<Matrix>(vec); // The vecMatrix generated should be n x 1 (column should be 1)
//     if (cols == vecMatrix.rows && vecMatrix.cols == 1)
//     {
//         Matrix result(this->rows, vecMatrix.cols);
//         for (int i = 0; i < this->rows; i++)
//             for (int j = 0; j < vecMatrix.cols; j++)
//                 for (int k = 0; k < this->cols; k++)
//                 {
//                     result.data[i][j] += this->data[i][k] * vecMatrix.data[k][j];
//                 }
//         return static_cast<Vector2D>(result);
//     }
//     return vec;
// }

Vector2D Matrix::operator*(const Vector2D &vec) const
{
    // Ensure we are only trying to rotate with a 2x2 matrix
    if (rows == 2 && cols == 2) 
    {
        // Direct algebraic expansion
        float newX = (data[0][0] * vec.m_x) + (data[0][1] * vec.m_y);
        float newY = (data[1][0] * vec.m_x) + (data[1][1] * vec.m_y);
        
        return Vector2D(newX, newY);
    }
    
    // Fallback if someone tries to multiply a Vector by a 3x3 or 4x4 matrix
    std::cout << "Error: Vector multiplication requires a 2x2 Matrix.\n";
    return vec; 
}   

Matrix Matrix::transpose() const
{
    Matrix result(cols, rows);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            result.data[j][i] = data[i][j];
        }
    }
    return result;
}

void Matrix::display() const
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            std::cout << data[i][j] << "  ";
        }
        std::cout << "\n";
    }
}