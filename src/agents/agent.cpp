#include "../../include/agents/agent.hpp"

Agent::Agent(int id, const Vector2D &startPosition) : m_id(id), m_position(startPosition), m_velocity(0, 0)
{
    // Note: If you want the agent to appear on the grid the moment it is created,
    // you will need to call env.placeAgent() in your main.cpp right after initializing the agent.
}

Vector2D Agent::getPosition() const
{
    return m_position;
}

void Agent::setVelocity(const Vector2D &velocity)
{
    m_velocity = velocity;
}

void Agent::move(Environment &env)
{
    Vector2D targetPosition = m_position + m_velocity;

    if (env.isInsideBounds(targetPosition) && !env.isObstacle(targetPosition))
    {
        env.clearCell(m_position);
        m_position = targetPosition;
        env.placeAgent(m_position); // placing agent in the environment
    }
    else
    {
        std::cout << "Agent " << m_id << " is blocked!\n";
    }
}