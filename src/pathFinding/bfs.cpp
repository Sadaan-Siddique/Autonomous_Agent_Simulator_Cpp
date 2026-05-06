#include "../../include/pathFinding/bfs.hpp"

std::vector<Vector2D> BFS::findPath(
    const Vector2D& start,
    const Vector2D& goal,
    const Environment& env)
{
    std::queue<Vector2D> q;
    std::unordered_set<Vector2D, Vector2DHash> visited;
    std::unordered_map<Vector2D, Vector2D, Vector2DHash> parent;

    q.push(start);
    visited.insert(start);

    std::vector<Vector2D> directions = {
        {1,0}, {-1,0}, {0,1}, {0,-1}
    };

    while (!q.empty())
    {
        Vector2D current = q.front();
        q.pop();

        if (current == goal)
            break;

        for (const auto& dir : directions)
        {
            Vector2D next = current + dir;

            if (!env.isInsideBounds(next))
                continue;

            if (env.isObstacle(next))
                continue;

            if (visited.count(next))
                continue;

            q.push(next);
            visited.insert(next);
            parent[next] = current;
        }
    }

    if (!visited.count(goal))
        return {};

    std::vector<Vector2D> path;
    Vector2D current = goal;

    while (!(current == start))
    {
        path.push_back(current);
        current = parent[current];
    }

    path.push_back(start);
    std::reverse(path.begin(), path.end());

    return path;
}