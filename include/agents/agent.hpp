#ifndef AGENT_H
#define AGENT_H

#include "../core/vector2d.hpp"
#include "../math/matrix.hpp"
#include "../environment/environment.hpp"
#include "../sensors/sensor.hpp"
// #include "../pathFinding/bfs.hpp" // For using BFS, uncomment this
#include "../pathFinding/aStar.hpp"

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

        std::vector<Vector2D> m_currentPointCloud; // Stores the latest LIDAR scan

    public:
        Agent(int, const Vector2D&, Sensor* );

        Vector2D getPosition() const;
        float getHeading() const;
        bool isUnreachable() const;   // Getter for the renderer

        void setVelocity(const Vector2D& );
        void setTarget(const Vector2D& target);

        // Method to wipe memory and restart
        void reset(const Vector2D& startPos);

        void move(Environment&, float deltaTime); // 'deltaTime' to properly calculate physics over time

        void sense(Environment& );

        // Add a getter so the Renderer can draw the dots
        std::vector<Vector2D> getPointCloud() const;

        // Autonomous Logic
        void decideNextMove(Environment&, float deltaTime);
        void chooseAlternativeDirection(Environment& );
        Vector2D computeDirectionToTarget() const;

        void computePath(Environment& env);


};

#endif