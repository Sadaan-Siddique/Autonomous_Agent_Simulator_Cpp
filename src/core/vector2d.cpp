#include "../../include/core/vector2d.hpp"
#include <cmath>

Vector2D::Vector2D(float x, float y) : m_x(x), m_y(y) {}

Vector2D Vector2D::operator+(const Vector2D &other) const
{
    return Vector2D(m_x + other.m_x, m_y + other.m_y);
}

Vector2D Vector2D::operator-(const Vector2D &other) const
{
    return Vector2D(m_x - other.m_x, m_y - other.m_y);
}

bool Vector2D::operator==(const Vector2D &other) const
{
    return (m_x == other.m_x && m_y == other.m_y);
}

Vector2D &Vector2D::operator+=(const Vector2D &other)
{
    m_x += other.m_x;
    m_y += other.m_y;
    return *this; // Returns a reference to the updated object
}

Vector2D Vector2D::operator*(float scalar) const
{
    return Vector2D(m_x * scalar, m_y * scalar);
}

std::ostream &operator<<(std::ostream &os, const Vector2D &v)
{
    os << "(" << v.m_x << ", " << v.m_y << ")";
    return os;
}

void Vector2D::move(const Vector2D &velocity)
{
    // We can just reuse our += operator to keep things DRY (Don't Repeat Yourself)
    *this += velocity;
}

double Vector2D::distance(const Vector2D &target) const
{
    float dx = m_x - target.m_x;
    float dy = m_y - target.m_y;
    return std::sqrt(dx * dx + dy * dy);
}

double Vector2D::magnitude() const
{
    return std::sqrt(m_x * m_x + m_y * m_y);
}
