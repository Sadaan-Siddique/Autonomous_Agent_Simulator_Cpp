#include "../../include/agents/agent.hpp"

Agent::Agent(int id, const Vector2D &startPosition, Sensor *sensor) : m_id(id), m_position(startPosition), m_velocity(0, 0), m_sensor(sensor)
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

void Agent::setTarget(const Vector2D& target)
{
    m_target = target;
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

int Agent::sense(Environment& env) const
{
    return m_sensor->detect(m_position, m_velocity, env);
}

void Agent::decideNextMove(Environment& env)
{
    if (m_position == m_target)
    {
        std::cout << "Target reached!\n";
        return;
    }

    Vector2D desiredDir = computeDirectionToTarget();

    // int obstacleDistance = m_sensor->detect(m_position, m_velocity, env);
    int obstacleDistance = m_sensor->detect(m_position, desiredDir, env);

    if (obstacleDistance == -1 || obstacleDistance > 1)
    {
        m_velocity = desiredDir;
        move(env); // Safe to move
    }
    else
    {
        chooseAlternativeDirection(env);
        move(env);
    }
}

void Agent::chooseAlternativeDirection(Environment& env)
{
    std::vector<Vector2D> directions = {
        {1,0}, {-1,0}, {0,1}, {0,-1} // For moving one step left, right bottom or top
    };

    for(const auto& dir : directions)
    {
        Vector2D nextPos = m_position + dir;

        if(env.isInsideBounds(nextPos) && !env.isObstacle(nextPos))
        {
            m_velocity = dir;
            return;
        }
    }
}

Vector2D Agent::computeDirectionToTarget() const 
{
    Vector2D delta = m_target - m_position;

    // Restrict movement to 4-way (Orthogonal) by picking the longer axis first
    if(std::abs(delta.m_x) > std::abs(delta.m_y))
        return Vector2D(((delta.m_x > 0) ? 1 : -1 ), 0); // Move Horizontally 
    else if(delta.m_y != 0)
        return Vector2D(0, ((delta.m_y > 0) ? 1 : -1)); // Move Vertically

    return Vector2D(0, 0);
}