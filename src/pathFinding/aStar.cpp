// Instead of a standard std::queue (First-In, First-Out), A* uses a std::priority_queue. It constantly sorts the memory so the cell with the lowest total cost (fCost) is always checked next.

#include "../../include/pathFinding/aStar.hpp"
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <functional> // For std::greater
#include <cmath>

// A helper struct specifically for the Priority Queue to sort by cost
struct NodeRecord
{
    Vector2D pos;
    float fCost;

    // We overload the > operator so the priority queue acts as a "Min-Heap"
    // (Lowest fCost stays at the top of the queue)
    bool operator>(const NodeRecord &other) const
    {
        return fCost > other.fCost;
    }
};

float AStar::getHeuristic(const Vector2D &a, const Vector2D &b)
{
    // We use standard Euclidean distance (straight line) as our heuristic guess
    return a.distance(b);
}

// std::vector<Vector2D> AStar::findPath(const Vector2D &start, const Vector2D &goal, const Environment &env)
std::vector<Vector2D> AStar::findPath(const Vector2D& start, const Vector2D& goal, const std::vector<std::vector<int>>& grid)
{
    if (grid.empty() || grid[0].empty()) return {}; // Safety check to prevent segfaults on empty arrays

    // 1. The Core Data Structures
    std::priority_queue<NodeRecord, std::vector<NodeRecord>, std::greater<NodeRecord>> openSet;
    std::unordered_set<Vector2D, Vector2DHash> closedSet;
    std::unordered_map<Vector2D, Vector2D, Vector2DHash> parent;

    // gCost tracks the exact distance traveled from the start to the current cell
    std::unordered_map<Vector2D, float, Vector2DHash> gCost;

    // Initialize start node
    openSet.push({start, getHeuristic(start, goal)});
    gCost[start] = 0.0f;

    std::vector<Vector2D> directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}, // Straight
        {1, 1}, {-1, -1}, {1, -1}, {-1, 1} // Diagonal
    };

    int height = grid.size();
    int width = grid[0].size();

    // 2. The Search Loop
    while (!openSet.empty())
    {
        Vector2D current = openSet.top().pos;
        openSet.pop();

        if (current == goal) break;

        // If we already evaluated this cell perfectly, skip it
        if (closedSet.count(current)) continue;
        closedSet.insert(current);

        for (const auto &dir : directions)
        {
            // Use (int) casting to match the physics engine's truncation!
            int nextX = (int)(current.m_x + dir.m_x);
            int nextY = (int)(current.m_y + dir.m_y);

            // --- GRID BOUNDARY CHECK ---
            if(nextX < 0 || nextX >= width || nextY < 0 || nextY >= height) continue;

            // --- GRID OBSTACLE CHECK ---
            // Only '1' is a wall. We assume Unknown (-1) is safe until the LIDAR proves otherwise!
            if(grid[nextY][nextX] == 1 ) continue;

            // --- STRICT DIAGONAL CORNER CLEARANCE ---
            if (dir.m_x != 0 && dir.m_y != 0) {
                int currX = (int)current.m_x; 
                int currY = (int)current.m_y; 
                // If either adjacent orthogonal cell is a known wall (1), block the diagonal
                if (grid[currY][nextX] == 1 || grid[nextY][currX] == 1) {
                    continue; 
                }
            }

            Vector2D next(nextX, nextY);

            // --- THE MATHEMATICAL COST UPGRADE ---
            // Diagonal moves cost 1.414, straight moves cost 1.0
            float stepCost = (dir.m_x != 0 && dir.m_y != 0) ? 1.4142f : 1.0f;
            float tentative_gCost = gCost[current] + stepCost;

            // If we found a faster, cheaper path to 'next', update its memory!
            if (gCost.find(next) == gCost.end() || tentative_gCost < gCost[next])
            {
                parent[next] = current;
                gCost[next] = tentative_gCost;

                // fCost = gCost (exact distance traveled) + hCost (guess to the finish line)
                float fCost = tentative_gCost + getHeuristic(next, goal);
                openSet.push({next, fCost});
            }
        }
    }

    if(gCost.find(goal) == gCost.end()) return {}; // unreachable
    
    // 3. Reconstruct the Path
    std::vector<Vector2D> path;
    Vector2D current = goal;
    while (!(current == start)) {
        path.push_back(current);
        current = parent[current];
    }
    
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}