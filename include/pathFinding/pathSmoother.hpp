// If you fed this raw A* path (till phase 5) directly into a real car's steering wheel, the car would violently jerk left and right trying to follow the zigzags.

// In professional autonomous systems (like ROS 2), A* is only the Global Planner. Once A* spits out this jagged grid path, the system passes those coordinates to a Local Planner or a Path Smoother.

// The Smoother applies algorithms like Bezier Curves, B-Splines, or Gradient Descent to "pull" the jagged corners tight, completely ignoring the grid and converting the waypoints into a beautiful, continuous, physics-friendly curve.

// If you feed a jagged, stair-step A* path to a real car, the steering servos will constantly snap left and right. The car will shake violently and burn out its motors. To fix this, we apply an algorithm to the waypoint array after A* finishes, but before the car starts driving.
// There are a few ways to do this, but the most elegant and common method for grid-based pathfinding is Gradient Descent Smoothing (often called the "Rubber Band" algorithm).


// How the "Rubber Band" Algorithm Works?
// Imagine your A* path is a string. The start and end waypoints are nailed to the floor. Every other waypoint in the middle is a loose peg.
// If you pull the string tight, it naturally wants to form a straight line. The algorithm does this mathematically by looping through every waypoint (except the first and last) and pulling it slightly toward the average position of its two neighbors.
// $$P_i = P_i + \alpha \cdot (P_{i-1} + P_{i+1} - 2P_i)$$
// Where:$P_i$ is the current waypoint.
// $\alpha$ is a "tension" factor (e.g., $0.5$).
// If you run this loop once, the sharp $90^\circ$ corners get slightly rounded. If you run the loop 50 times, the entire path relaxes into a beautiful, continuous, aerodynamic curve.

// Data Weight α: Pulls the waypoint toward the original safe A* grid coordinate.
// Smooth Weight β: Pulls the waypoint toward its neighbors to make it smooth.



// The Math: The Two Competing Forces
// Imagine the jagged A* path is a series of waypoints: $P_0, P_1, P_2... P_n$.
// We want to create a brand new, smoothed path (let's call the new waypoints $S_0, S_1, S_2... S_n$)
//     .The algorithm loops over every waypoint (skipping the start and end, because we can't move the car's physical location or the target destination). For every point in the middle, it applies two competing mathematical forces:
//     1. The Data Weight α - "The Seatbelt"
//         We don't want the smooth path to drift too far from the original A* path, because the A* path is guaranteed to be safe from walls. The Data Weight pulls the new smooth point ($S_i$) back toward the original, safe A* point ($P_i$).
//         Math: $\alpha \cdot (P_i - S_i)$
//     2. The Smoothing Weight β - "The Rubber Band"
//         We want the line to be straight and tight. The Smoothing Weight looks at the neighbor behind the point ($S_{i-1}$) and the neighbor ahead of the point ($S_{i+1}$) and pulls the current point toward their average center.
//         Math: $\beta \cdot (S_{i-1} + S_{i+1} - 2S_i)$
//     The Final Equation:
//     We apply both forces to the point simultaneously, update it, and repeat the loop until the points stop moving.
//     S_i = S_i + α * (P_i - S_i) + β(S_{i-1} + S_{i+1} - 2S_i)

#ifndef PATH_SMOOTHER_H
#define PATH_SMOOTHER_H

#include <vector>
#include "../core/vector2d.hpp"

class PathSmoother { // Using Gradient Descent Smoothing (often called the "Rubber Band" algorithm) algorithm to smoothen the trajectory
    public:
    // Takes the raw A* path, applies the math, and returns a curvy, physics-friendly path
    static std::vector<Vector2D> smoothPath(
        const std::vector<Vector2D>& originalPath, 
        float dataWeight,    // Alpha (e.g., 0.1)
        float smoothWeight,  // Beta  (e.g., 0.5)
        float tolerance      // When to stop calculating (e.g., 0.001)
    );

};

#endif