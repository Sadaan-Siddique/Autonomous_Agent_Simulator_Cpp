#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <iostream>
#include <vector>
// #include "../agents/agent.hpp"
#include "../core/vector2d.hpp"
#include "./dynamicObstacle.hpp"

enum class CellType {
    EMPTY,
    OBSTACLE,
    AGENT,
};

class Environment {
private:
    int m_width;
    int m_height;
    std::vector<std::vector<int>> m_grid;
    // // Initialize 5 rows, where each row is a vector of 10 integers(10 columns) set to default value 0
    // std::vector<std::vector<int>> matrix(5, std::vector<int>(10, 0));

    std::vector<DynamicObstacle> m_dynamicObstacles;    // List to hold all moving obstacles 

public:
    Environment(int, int );

    // bool isInsideBounds(int x, int y) const; // Check Bounds
    // bool isObstacle(int x, int y) const;
    bool isInsideBounds(const Vector2D& ) const;
    bool isObstacle(const Vector2D& ) const;
    bool isStaticObstacle(const Vector2D &pos) const;

    // void placeObstacle(int x, int y);
    // void clearCell(int x, int y);
    void placeObstacle(const Vector2D& );
    void placeAgent(const Vector2D& );
    void clearCell(const Vector2D& pos);

    int getWidth() const;
    int getHeight() const;
    std::vector<std::vector<int>> getGrid() const;

    // void placeRandomObstacles(int count, const Vector2D& currentPos, const Vector2D& targetLocation);
    void placeRandomObstacles(int count);

    void printGrid() const;

    void updateDynamicObstacles(float deltaTime, const Vector2D& agentPos);
    const std::vector<DynamicObstacle>& getDynamicObstacles() const;
};

#endif