#include "../../include/pathFinding/pathSmoother.hpp"
#include <cmath>

// --- GRADIENT DESCENT PATH SMOOTHER ---
// A* Pathfinding produces "jagged" paths because it is restricted to moving center-to-center on a square grid.
// This algorithm acts like a "rubber band". It takes the jagged integer path and pulls it tight, 
// rounding off the harsh 90-degree and 45-degree turns into smooth, drivable curves for the robotic kinematics.
std::vector<Vector2D> PathSmoother::smoothPath(const std::vector<Vector2D> &originalPath, const std::vector<std::vector<int>>& grid, float dataWeight, float smoothWeight, float tolerance)
{
    // If the path is just a start and end point (a straight line), it is already as smooth as possible!
    if (originalPath.size() <= 2)
    {
        return originalPath;
    }

    // --- 1. PATH UPSAMPLING (Anti-Corner Cutting) ---
    // Problem: If you pull a rubber band tight across a sharp 90-degree hallway corner, the band will 
    // cut diagonally straight through the wall's physical collision box.
    // Solution: We inject artificial "midpoints" between every A* waypoint. This gives the rubber band 
    // enough geometric resolution (bones) to smoothly curve *around* the corner instead of slicing through it.
    std::vector<Vector2D> densePath;
    for (size_t i = 0; i < originalPath.size() - 1; i++) {
        densePath.push_back(originalPath[i]);
        Vector2D midPoint((originalPath[i].m_x + originalPath[i+1].m_x) / 2.0f,
                          (originalPath[i].m_y + originalPath[i+1].m_y) / 2.0f);
        densePath.push_back(midPoint);
    }
    // Always preserve the absolute final target destination
    densePath.push_back(originalPath.back());

    // Create a working copy of the upsampled path that we will iteratively modify
    std::vector<Vector2D> newPath = densePath;
    
    // Tracks how much the entire path shifted in a single pass. 
    // When the path barely moves anymore (change < tolerance), we know the curve has stabilized.
    float change = tolerance;

    int height = grid.size();
    int width = grid[0].size();

    // --- 2. THE GRADIENT DESCENT LOOP ---
    // Continuously apply mathematical tension to the path until it settles into a smooth curve.
    while (change >= tolerance)
    {
        change = 0.0f;

        // Loop through all intermediate waypoints (Skipping the Start and the End anchors so they never move)
        for (size_t i = 1; i < densePath.size() - 1; i++)
        {
            Vector2D oldPos = newPath[i];

            // --- THE MATHEMATICAL FORCES ---
            // Force 1 (Data Weight / Alpha): The "Anchor" force. This pulls the waypoint back toward its 
            // original A* position. Without this, the path would completely collapse into a straight line and hit walls.
            // Force 2 (Smooth Weight / Beta): The "Tension" force. This pulls the waypoint toward the geometric average 
            // of its immediate predecessor [i-1] and successor [i+1], smoothing out jagged spikes.
            float nextX = newPath[i].m_x + dataWeight * (densePath[i].m_x - newPath[i].m_x) +
                          smoothWeight * (newPath[i-1].m_x + newPath[i+1].m_x - (2.0f * newPath[i].m_x));

            float nextY = newPath[i].m_y + dataWeight * (densePath[i].m_y - newPath[i].m_y) +
                          smoothWeight * (newPath[i-1].m_y + newPath[i+1].m_y - (2.0f * newPath[i].m_y));

            // --- 3. THE MAXIMUM SAFETY BOX (Collision Check) ---
            // Just because the math says a smooth curve looks nice doesn't mean it's physically safe to drive on!
            // We project a virtual bounding box (0.49 units wide) around this proposed new smooth coordinate.
            // If this box touches an orange wall (1), the shift is rejected, acting as a magnetic repulsion field from obstacles.
            float margin = 0.42f; 
            bool isSafe = true;

            // Probe a 3x3 grid centered on the new coordinate to guarantee absolute physical clearance
            for (int ix = -1; ix <= 1; ix++) {
                for (int iy = -1; iy <= 1; iy++) {
                    
                    float ox = ix * margin; 
                    float oy = iy * margin;
                    
                    int checkX = (int)(nextX + ox);
                    int checkY = (int)(nextY + oy);
                    
                    // Out-of-bounds or Obstacle check
                    if (checkX < 0 || checkX >= width || checkY < 0 || checkY >= height || grid[checkY][checkX] == 1) {
                        isSafe = false;
                        break;
                    }
                }
                if (!isSafe) break;
            }

            // --- 4. APPLY VALIDATED SHIFT ---
            // If the smoothed point doesn't scrape a wall, permanently apply the change to the path array.
            if (isSafe) {
                newPath[i].m_x = nextX;
                newPath[i].m_y = nextY;
                
                // Accumulate the total physical distance the points shifted during this iteration
                change += std::abs(oldPos.m_x - newPath[i].m_x);
                change += std::abs(oldPos.m_y - newPath[i].m_y);
            }
        }
    }
    
    // Return the physically validated, rubber-banded continuous trajectory
    return newPath;
}