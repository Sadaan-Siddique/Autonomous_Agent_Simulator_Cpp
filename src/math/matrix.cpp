#include "../../include/math/matrix.hpp"
#include <iostream>
#include <cmath>
#include <algorithm> // std::swap ke liye

Matrix::Matrix(int r, int c) : rows(r), cols(c)
{
    data.resize(rows, std::vector<double>(cols, 0.0));
}

double Matrix::getValue(int r, int c) const {
    if (r < rows && c < cols) return data[r][c];
    return 0.0;
}

void Matrix::setValue(int r, int c, double value)
{
    if (r < rows && c < cols)
        data[r][c] = value;
}

Matrix::Matrix(const Vector2D &vec)
{
    // Vector2D ko default 3x1 homogeneous vector banayenge taake future math asaan ho
    rows = 2;
    cols = 1;

    // MUST allocate the memory first!
    data.resize(rows, std::vector<double>(cols, 0.0));

    data[0][0] = vec.m_x; // X goes in Row 0, Col 0
    data[1][0] = vec.m_y; // Y goes in Row 1, Col 0
    data[2][0] = 1.0;     // w = 1 (Homogeneous coordinate)
}

Matrix::operator Vector2D() const
{
    // Safety check: We can only make a Vector2D out of a 2x1 Matrix
    if (rows == 2 && cols == 1)
    {
        // Extract X from row 0, Y from row 1
        return Vector2D(data[0][0], data[1][0]);
    }

    // Homogeneous 3x1 to 2D Vector Conversion
    else if (rows == 3 && cols == 1)
    {
        double w = data[2][0];
        // Cartesian coordinates mein wapas laane ke liye 'w' se divide karte hain
        if (w != 0.0 && w != 1.0)
        {
            return Vector2D(data[0][0] / w, data[1][0] / w);
        }
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

Matrix Matrix::operator-(const Matrix &other) const {
    Matrix result(rows, cols);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result.data[i][j] = this->data[i][j] - other.data[i][j];
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
    // 1. Vector ko N-Dimensional column format mein laayein
    std::vector<double> vecData(cols, 0.0); // Initialize with 0.0

    if (cols > 0)
        vecData[0] = vec.m_x; // Pehli jagah par X
    if (cols > 1)
        vecData[1] = vec.m_y; // Doosri jagah par Y

    // Agar matrix 3x3 ya us se bari hai, to aakhri position par Homogeneous 'w' (1.0) rakh dein
    if (cols > 2)
        vecData[cols - 1] = 1.0;

    // 2. N x N Mathematical Multiplication Loop
    std::vector<double> resultData(rows, 0.0);

    for (int i = 0; i < rows; i++) // Har row ke liye
    {
        for (int j = 0; j < cols; j++) // Har column ke liye
        {
            // Dot Product: Matrix ka cell * Vector ka cell
            resultData[i] += data[i][j] * vecData[j];
        }
    }

    // 3. N-Dimensional Result ko wapas 2D Vector (X, Y) mein extract karein
    double newX = (rows > 0) ? resultData[0] : 0.0;
    double newY = (rows > 1) ? resultData[1] : 0.0;

    // Homogeneous Division (Agar matrix ne 'w' ko modify kiya hai, to coordinate ko w se divide karein)
    if (rows > 2)
    {
        double w = resultData[rows - 1]; // Result ki aakhri value 'w' hai

        if (w != 0.0 && w != 1.0)
        {
            newX /= w;
            newY /= w;
        }
    }

    return Vector2D(newX, newY);
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


Matrix Matrix::inverse() const
{
    // STEP 0: Safety Check
    // Inverse sirf "Square" matrices (jaise 2x2, 3x3, 4x4) ka nikalta hai.
    if (rows != cols)
    {
        std::cout << "Error: Only square matrices can be inverted.\n";
        return Matrix(rows, cols);
    }

    int n = rows;

    // STEP 1: Augmentation [A | I]
    // Ek temporary 2D vector banate hain jiski width double hogi (n x 2n)
    std::vector<std::vector<double>> aug(n, std::vector<double>(2 * n, 0.0));

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            aug[i][j] = data[i][j]; // Left side: Original Matrix (A)
        }
        aug[i][i + n] = 1.0; // Right side: Identity Matrix (I)
    }

    // STEP 2: Gauss-Jordan Row Operations
    for (int i = 0; i < n; i++)
    {
        // 2A: Pivoting (Sab se bara number dhoondna)
        // Agar diagonal par 0 aa jaye to math fail (divide by zero) ho jati hai.
        // Isliye hum neeche wali rows mein se sab se bari value wali row ko oopar le aate hain.
        double maxEl = std::abs(aug[i][i]);
        int maxRow = i;
        for (int k = i + 1; k < n; k++) // ye loop ith column ki hr value check karega
        {
            if (std::abs(aug[k][i]) > maxEl)
            {
                maxEl = std::abs(aug[k][i]);
                maxRow = k;
            }
        }

        // Rows ko swap kar diya taake division safe rahe
        std::swap(aug[i], aug[maxRow]);

        // Agar sab se bari value bhi 0 hai, iska matlab matrix "Singular" hai q ke iska matlab poora ith column hi zero hai
        // (Yani is matrix ka inverse mathematics ki duniya mein exist hi nahi karta).
        // Instead of: if (aug[i][i] == 0.0)
        // Use a tiny threshold like 1e-9 (0.000000001)
        if (std::abs(aug[i][i]) < 1e-9)
        {
            std::cout << "Error: Matrix is singular (Determinant is 0) and cannot be inverted.\n";
            return Matrix(n, n);
        }

        // 2B: Make Pivot 1
        // Current row ki diagonal value ko 1 banane ke liye, poori row ko us value se divide kar do.
        double pivot = aug[i][i];
        for (int j = 0; j < 2 * n; j++)
        {
            aug[i][j] /= pivot;
        }

        // 2C: Make other values in column 0
        // Ab is 1 ko use kar ke baqi saari rows (oopar aur neeche) ki same column ki values ko 0 kar do.
        for (int k = 0; k < n; k++)
        {
            if (k != i) // Khud apni row ko minus nahi karna
            {
                double factor = aug[k][i]; // Jo value 0 karni hai
                for (int j = 0; j < 2 * n; j++)
                {
                    // R_k = R_k - (factor * R_i)
                    aug[k][j] -= factor * aug[i][j];
                }
            }
        }
    }

    // STEP 3: Extraction [I | A^-1]
    // Ab left side Identity ban chuki hai. Hum right side wala half nikal kar result mein daal lenge.
    Matrix result(n, n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            result.setValue(i, j, aug[i][j + n]); // 'j + n' se right side ka data milta hai
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

// STATIC FACTORY METHODS (Robotics Standard)

Matrix Matrix::identity(int size)
{
    Matrix result(size, size);
    for (int i = 0; i < size; i++)
        result.setValue(i, i, 1.0);
    return result;
}

Matrix Matrix::translation(double tx, double ty)
{
    // you do not need to use the Class:: prefix if you are calling a static function from inside another static function of the same class. When you are writing code inside a member function (static or non-static), you are already within the scope of that class. The compiler automatically knows where to look for other members of the same class
    Matrix result = identity(3);
    result.setValue(0, 2, tx);
    result.setValue(1, 2, ty);
    return result;
}

Matrix Matrix::rotation(double angleRadians)
{
    Matrix result = identity(3);

    result.setValue(0, 0, std::cos(angleRadians));
    result.setValue(0, 1, -std::sin(angleRadians));
    result.setValue(1, 0, std::sin(angleRadians));
    result.setValue(1, 1, std::cos(angleRadians));
    return result;
}