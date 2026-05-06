#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <vector>

class Vector2D
{
public:
    int m_x;
    int m_y;

    Vector2D(int x = 0, int y = 0);

    Vector2D operator+(const Vector2D &) const; // position + direction
    Vector2D operator-(const Vector2D &) const;
    bool operator==(const Vector2D &) const;
    Vector2D &operator+=(const Vector2D &);
    Vector2D operator*(int) const;
    friend std::ostream &operator<<(std::ostream &, const Vector2D &);

    void move(const Vector2D &velocity);

    double distance(const Vector2D &) const;

    double magnitude() const;
};

#endif