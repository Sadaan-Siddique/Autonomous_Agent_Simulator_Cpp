// BFS (Breadth First Search) is like a blind flood of water; it searches in every possible direction equally until it happens to hit the target. It is incredibly slow and inefficient for large maps.
#ifndef BFS_H
#define BFS_H

#include "../core/vector2d.hpp"
#include "../environment/environment.hpp"
#include <queue> // Implements FIFO (First In First Out) 🧠 In BFS Explore nodes in order: start → neighbors → neighbors of neighbors
#include <unordered_map> // Stores unique elements with fast lookup. 🧠 In BFS Visited nodes Why needed? Prevents: Infinite loops Revisiting same node
#include <unordered_set> // Stores key → value pairs In BFS child → parent  Example: (3,3) → (2,3) (2,3) → (1,3), needed to reconstruct path: goal → parent → parent → start 
#include <algorithm> // Provides utility functions In BFS We reverse path because: We build: goal → ... → start But we want: start → ... → goal
#include <vector>

class BFS {
    public:
        static std::vector<Vector2D> findPath(const Vector2D& start, const Vector2D& goal, const Environment& env);
};

#endif

// WHY BFS GIVES SHORTEST PATH
// Because it explores level by level:
// Distance 1 nodes
// Distance 2 nodes
// Distance 3 nodes
// So the first time it reaches the goal:
// 👉 That path is guaranteed shortest (in grid with equal cost)

// BFS explores the grid like ripples in water

// 🔁 Step-by-Step Logic
// ✅ Step 1: Start from Agent Position
// Queue = [start]
// Visited = {start}

// ✅ Step 2: Explore Neighbors
// From current cell, check:
// Up    → (0, -1)
// Down  → (0, +1)
// Left  → (-1, 0)
// Right → (1, 0)

// ✅ Step 3: For Each Neighbor
// Check:
// 1. Is inside bounds?
// 2. Not obstacle?
// 3. Not already visited?
// If YES:
// → Add to queue
// → Mark visited
// → Store parent

// ✅ Step 4: Stop When Target Found
// Once:
// current == target
// 👉 BFS stops

// ✅ Step 5: Reconstruct Path
// Now we backtrack using parent map:
// target → parent → parent → ... → start
// Then reverse it:
// start → ... → target

// 🧱 Data Structures Used
// 1. Queue
// Stores nodes to explore next
// FIFO → First In First Out

// 2. Visited Set
// Prevents infinite loops

// 3. Parent Map
// child → parent
// Used to reconstruct path later

// ⚠️ PROBLEM (VERY IMPORTANT)
// BFS assumes:
// Environment is static (nothing changes)

// But in your system:
// 👉 Environment can change
// 👉 Obstacles may appear
// 👉 Path may get blocked

// 🔄 Full Logic Flow Now
// Step-by-step
// 1. Agent has a path
// 2. Wants to move forward
// 3. Sensor checks forward cell
// 4. If blocked:
//        → recompute path using BFS
// 5. Else:
//        → continue moving
// 🧠 BIG IDEA
// BFS = Brain (Planning)
// Find best path globally
// Sensor = Eyes (Perception)
// Detect local changes
// Agent = Body (Execution)
// Move step by step