#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <iostream>
#include <vector>
#include "../core/vector2d.hpp"

enum class CellType {
    EMPTY,
    OBSTACLE,
    AGENT,
    TARGET,
    PATH,
};

class Environment {
private:
    int m_width;
    int m_height;
    std::vector<std::vector<int>> m_grid;
    // // Initialize 5 rows, where each row is a vector of 10 integers(10 columns) set to default value 0
    // std::vector<std::vector<int>> matrix(5, std::vector<int>(10, 0));

public:
    Environment(int, int );

    // bool isInsideBounds(int x, int y) const; // Check Bounds
    // bool isObstacle(int x, int y) const;
    bool isInsideBounds(const Vector2D& ) const;
    bool isObstacle(const Vector2D& ) const;

    // void placeObstacle(int x, int y);
    // void clearCell(int x, int y);
    void placeObstacle(const Vector2D& );
    void placeAgent(const Vector2D& );
    void clearCell(const Vector2D& pos);

    int getWidth() const;
    int getHeight() const;

    void placeRandomObstacles(int count);

    void printGrid() const;
};

#endif