#ifndef AGENT_H
#define AGENT_H

#include "../core/vector2d.hpp"
#include "../math/matrix.hpp"
#include "../environment/environment.hpp"
#include "../sensors/sensor.hpp"
#include "../pathFinding/bfs.hpp"

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

        // Continuous Physical Properties
        float m_headingAngle; // The angle the agent is currently facing (in radians)
        float m_speed; // How fast the agent drives (units per second)


    public:
        Agent(int, const Vector2D&, Sensor* );

        Vector2D getPosition() const;

        void setVelocity(const Vector2D& );
        void setTarget(const Vector2D& target);

        void move(Environment&, float deltaTime); // 'deltaTime' to properly calculate physics over time

        int sense(Environment& ) const;

        // Autonomous Logic
        void decideNextMove(Environment&, float deltaTime);
        void chooseAlternativeDirection(Environment& );
        Vector2D computeDirectionToTarget() const;

        void computePath(Environment& env);

};

#endif