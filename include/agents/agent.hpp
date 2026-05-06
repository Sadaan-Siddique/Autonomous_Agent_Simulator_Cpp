#ifndef AGENT_H
#define AGENT_H

#include "../core/vector2d.hpp"
#include "../environment/environment.hpp"

// Using Composition and aggregations
class Agent {
    private:
        int m_id {};
        Vector2D m_position; // Stores where agent currently is
        Vector2D m_velocity; // Stores movement direction/speed

    public:
        Agent(int, const Vector2D& );

        Vector2D getPosition() const;

        void setVelocity(const Vector2D& );

        void move(Environment& );
};

#endif