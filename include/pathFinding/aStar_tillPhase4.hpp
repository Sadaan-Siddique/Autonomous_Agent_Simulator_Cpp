// Dijkstra vs. A* (Which is better?)
// For your autonomous agent, A (A-Star) is absolutely, undeniably better.*
// To understand why, you need to understand how these algorithms "think":
//     BFS (Your current brain): Searches blindly in all directions equally. It assumes moving up 1 square costs the same as moving diagonally 1 square. It stops when it accidentally bumps into the target.
//     Dijkstra: Also searches in all directions equally (like a ripple in a pond). However, it tracks the Cost of movement. It knows diagonals cost 1.414 and straight lines cost 1.0. It will find the perfect shortest path, but it is incredibly slow because it searches the whole map.
//     A (The Gold Standard):* A* is Dijkstra, but it has a Heuristic (a mathematical guess). Instead of searching everywhere, it looks at the target coordinate and says, "I know the target is to my Top-Right, so I will prioritize checking the Top-Right cells first." A* acts like a magnet pulling the search toward the goal. It finds the exact same mathematically perfect path as Dijkstra, but usually does it by checking 70% fewer cells.

#ifndef ASTAR_H
#define ASTAR_H

#include <vector>
#include "../core/vector2d.hpp"
#include "../environment/environment.hpp"

class AStar
{
public:
    // The main pathfinding function
    static std::vector<Vector2D> findPath(const Vector2D &start, const Vector2D &goal, const Environment &env);

private:
    // The "Heuristic" (The magnet that pulls A* toward the target)
    //  mental shortcut or "rule of thumb" that allows people to solve problems, make judgments, or make decisions quickly and with minimal mental effort.
    static float getHeuristic(const Vector2D &a, const Vector2D &b);
};

#endif
