#include "../../include/pathFinding/pathSmoother.hpp"
#include <cmath>

std::vector<Vector2D> PathSmoother::smoothPath(const std::vector<Vector2D> &originalPath, float dataWeight, float smoothWeight, float tolerance)
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

    // 2. The Gradient Descent Loop
    while (change >= tolerance)
    {
        change = 0.0f;

        // Loop through all points EXCEPT the Start (index 0) and the End (index size-1)
        for (size_t i = 1; i < originalPath.size() - 1; i++)
        {
            // Store the old position before we modify it so we can measure how far it moved
            Vector2D aux = newPath[i];

            // --- THE MATHEMATICAL FORCES --- | S_i = S_i + α * (P_i - S_i) + β(S_{i-1} + S_{i+1} - 2S_i)
            // Apply Data Weight (Alpha) and Smooth Weight (Beta) to the X coordinate
            newPath[i].m_x += dataWeight * (originalPath[i].m_x - newPath[i].m_x) + smoothWeight * (newPath[i - 1].m_x + newPath[i + 1].m_x - (2.0f * newPath[i].m_x));

            // Apply Data Weight (Alpha) and Smooth Weight (Beta) to the Y coordinate
            newPath[i].m_y += dataWeight * (originalPath[i].m_y - newPath[i].m_y) + smoothWeight * (newPath[i - 1].m_y + newPath[i + 1].m_y - (2.0f * newPath[i].m_y));

            // Accumulate the total distance this point shifted during this iteration
            change += std::abs(aux.m_x - newPath[i].m_x);
            change += std::abs(aux.m_y - newPath[i].m_y);

        }
    }
    return newPath;
}