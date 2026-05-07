#include "../../include/pathFinding/bfs.hpp"

std::vector<Vector2D> BFS::findPath(
    const Vector2D& start,
    const Vector2D& goal,
    const Environment& env)
{
    // This initializes the three core memory structures the algorithm needs to function.
    std::queue<Vector2D> q; // q is the list of cells we need to check next. The first person to get in line is the first person to be served.
    std::unordered_set<Vector2D, Vector2DHash> visited; // visited is the memory of cells we have already looked at so we don't check them twice.
    std::unordered_map<Vector2D, Vector2D, Vector2DHash> parent; // parent is our trail of breadcrumbs (who came from where).

    // We put the starting position into our queue to check, and immediately mark it as "visited" so we don't accidentally loop back to the start.
    q.push(start);
    visited.insert(start);

    // Defines the four orthogonal directions the agent is allowed to look (Right, Left, Down, Up).
    std::vector<Vector2D> directions = {
        {1,0}, {-1,0}, {0,1}, {0,-1}
    };

    // The Search Loop (The "Water Flood")
    while (!q.empty()) // Keep exploring as long as there are still cells in the queue waiting to be checked." If the queue becomes empty, it means we searched the whole map and got trapped.
    {
        // Grab the very first cell waiting in line (q.front()), save it as current, and then remove it from the line (q.pop()).
        Vector2D current = q.front();
        q.pop();

        if (current == goal) // If the cell we are currently standing on is our target, stop searching immediately! We found it!
            break;

        for (const auto& dir : directions) // Look at all 4 neighboring cells around our current cell. Calculate the coordinate of that neighboring cell and call it next.
        {
            Vector2D next = current + dir; 

            // The filter. If the next cell is off the map, is a wall (#), or we have already visited it in the past, continue (skip it and move to the next direction).
            if (!env.isInsideBounds(next))
                continue;

            if (env.isObstacle(next))
                continue;

            if (visited.count(next))
                continue;

            // If the neighbor passed all the filters, it's a valid empty space! Put it in the queue to be explored later, mark it as visited, and most importantly: record in the parent map that we reached next from current.
            q.push(next);
            visited.insert(next);
            parent[next] = current;
        }
    }

    if (!visited.count(goal)) // The loop finished. Check if we ever actually visited the goal. If we didn't, the maze is impossible. Return an empty list {}.
        return {};

    std::vector<Vector2D> path; // Create an empty list to store our final route, and start at the finish line (goal).
    Vector2D current = goal;

    // Walk backward. "While we aren't at the start yet, add our current location to the path, then look at our map to see which cell we came from, and step backward into it."
    while (!(current == start))
    {
        path.push_back(current);
        current = parent[current];
    }

    // Add the starting cell to the list. Because we traced the path backward (Goal -> Start), we reverse the list so the agent can read it forward (Start -> Goal). Give the list to the agent!
    path.push_back(start);
    std::reverse(path.begin(), path.end());

    return path;
}