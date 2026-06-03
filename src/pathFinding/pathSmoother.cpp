#include "../../include/pathFinding/pathSmoother.hpp"
#include <cmath>

std::vector<Vector2D> PathSmoother::smoothPath(const std::vector<Vector2D> &originalPath, const std::vector<std::vector<int>>& grid, float dataWeight, float smoothWeight, float tolerance)
{
    // If the path is just a start and end point (a straight line), it can't be smoothed!
    if (originalPath.size() <= 2)
    {
        return originalPath;
    }

    // 1. Create a copy of the path that we are allowed to modify
    std::vector<Vector2D> newPath = originalPath;

    // We track how much the points move. If they move less than our tolerance, we are done!
    float change = tolerance;

    int height = grid.size();
    int width = grid[0].size();

    // 2. The Gradient Descent Loop
    while (change >= tolerance)
    {
        change = 0.0f;

        // Loop through all points EXCEPT the Start (index 0) and the End (index size-1)
        for (size_t i = 1; i < originalPath.size() - 1; i++)
        {
            // Store the old position before we modify it so we can measure how far it moved
            Vector2D oldPos = newPath[i];

            // --- THE MATHEMATICAL FORCES --- | S_i = S_i + α * (P_i - S_i) + β(S_{i-1} + S_{i+1} - 2S_i)
            // Apply Data Weight (Alpha) and Smooth Weight (Beta) to the X coordinate
            float nextX = newPath[i].m_x + dataWeight * (originalPath[i].m_x - newPath[i].m_x) +
                          smoothWeight * (newPath[i-1].m_x + newPath[i+1].m_x - (2.0f * newPath[i].m_x));

            // Apply Data Weight (Alpha) and Smooth Weight (Beta) to the Y coordinate
            float nextY = newPath[i].m_y + dataWeight * (originalPath[i].m_y - newPath[i].m_y) +
                          smoothWeight * (newPath[i-1].m_y + newPath[i+1].m_y - (2.0f * newPath[i].m_y));

            // --- THE SAFETY CHECK ---
            // Only update the path if the new smoothed point is NOT inside a wall
            int gridX = (int)nextX;
            int gridY = (int)nextY;

            if (gridX >= 0 && gridX < width && gridY >= 0 && gridY < height) {
                if (grid[gridY][gridX] != 1) { // If it's not a wall
                    newPath[i].m_x = nextX;
                    newPath[i].m_y = nextY;
                    change += std::abs(oldPos.m_x - newPath[i].m_x);
                    change += std::abs(oldPos.m_y - newPath[i].m_y);
                }
            }

        }
    }
    return newPath;
}