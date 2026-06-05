#ifndef AGENT_H
#define AGENT_H

#include "../core/vector2d.hpp"
#include "../math/matrix.hpp"
#include "../environment/environment.hpp"
#include "../sensors/sensor.hpp"
// #include "../pathFinding/bfs.hpp" // For using BFS, uncomment this
#include "../pathFinding/aStar.hpp"
#include "../pathFinding/pathSmoother.hpp"

// Using Composition and aggregations
class Agent {
    private:
        int m_id {};
        Vector2D m_position; // Stores where agent currently is
        Vector2D m_velocity; // Stores movement direction/speed
        Vector2D m_target;
        Sensor* m_sensor;
        
        std::vector<Vector2D> m_path; // full path from start → target
        int m_pathIndex = 0; // where agent currently is in path
        bool m_usePathfinding = true;
        bool m_isUnreachable = false; // NEW: Panic flag

        // Continuous Physical Properties
        float m_headingAngle; // The angle the agent is currently facing (in radians)
        float m_speed; // How fast the agent drives (units per second)
        float m_angularVelocity; // How fast the agent is currently spinning

        // PID Controller Variables
        float m_kp, m_ki, m_kd; // PID constants. Tuning these three numbers (Kp, Ki, Kd) changes how aggressive or sluggish the car feels.
        float m_previousError;
        float m_integral;

        std::vector<std::pair<Vector2D, bool>> m_currentPointCloud; // Stores the latest LIDAR scan
        std::vector<std::vector<int>> m_internalMap; // The Agent's memory for SLAM Mapping

        // Bresenham's algorithm to trace LIDAR rays, The algorithm's job is to look at the start and end points, and figure out every single grid cell the laser passed through in between them.
        // By making it private, you guarantee that the only thing allowed to alter the agent's memory is the agent's own LIDAR sensor.
        void bresenhamTrace(int x0, int y0, int x1, int y1, bool isHit);
        // x0, y0 (Start): This is the Agent's exact physical location on the grid. It is the origin point of the LIDAR laser.
        // x1, y1 (End): This is where the LIDAR laser finally stopped. It either hit a wall, or it reached its maximum range and faded out into empty space.

    public:
        // For SLAM, agent needs to know how big its memory array should be 
        Agent(int id, const Vector2D& startPosition, Sensor* sensor, int mapWidth, int mapHeight);

        Vector2D getPosition() const;
        float getHeading() const;
        bool isUnreachable() const;   // Getter for the renderer
        std::vector<std::pair<Vector2D, bool>> getPointCloud() const; // Add a getter so the Renderer can draw the dots
        const std::vector<std::vector<int>>& getInternalMap() const;

        void setVelocity(const Vector2D& );
        void setTarget(const Vector2D& target);

        // Method to wipe memory and restart
        void reset(const Vector2D& startPos);

        void move(Environment&, float deltaTime); // 'deltaTime' to properly calculate physics over time

        void sense(Environment& );

        // Autonomous Logic
        void decideNextMove(Environment&, float deltaTime);

        void computePath(Environment& env);

};

#endif