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

void Agent::setTarget(const Vector2D &target)
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

int Agent::sense(Environment &env) const
{
    return m_sensor->detect(m_position, m_velocity, env);
}

void Agent::decideNextMove(Environment &env)
{
    // if (m_position == m_target)
    // {
    //     std::cout << "Target reached!\n";
    //     return;
    // }

    // Vector2D desiredDir = computeDirectionToTarget();

    // // int obstacleDistance = m_sensor->detect(m_position, m_velocity, env);
    // int obstacleDistance = m_sensor->detect(m_position, desiredDir, env);

    // if (obstacleDistance == -1 || obstacleDistance > 1)
    // {
    //     m_velocity = desiredDir;
    //     move(env); // Safe to move
    // }
    // else
    // {
    //     chooseAlternativeDirection(env);
    //     move(env);
    // }

    if (m_position == m_target)
    {
        std::cout << "Target reached!\n";
        return;
    }

    // 🔥 SWITCH HERE
    if (m_usePathfinding)
    {
        // --- BFS PATH FOLLOWING ---

        if (m_path.empty() || m_pathIndex >= m_path.size())
        {
            computePath(env);

            // NEW FIX: If it's STILL empty after computing, just stop.
            if (m_path.empty())
                return;
        }

        Vector2D nextPos = m_path[m_pathIndex];

        m_velocity = nextPos - m_position;

        if (sense(env) == 1)
        {
            std::cout << "Obstacle detected! Recomputing...\n";
            computePath(env);
            return;
        }

        move(env);
        m_pathIndex++;
    }
    else
    {
        // --- OLD GREEDY LOGIC ---
        Vector2D desiredDir = computeDirectionToTarget();

        int obstacleDistance = m_sensor->detect(m_position, desiredDir, env);

        if (obstacleDistance == -1 || obstacleDistance > 1)
        {
            m_velocity = desiredDir;
            move(env);
        }
        else
        {
            chooseAlternativeDirection(env);
            move(env);
        }
    }

    // if (!m_path.empty() && m_pathIndex < m_path.size())
    // {
    //     Vector2D next = m_path[m_pathIndex];
    //     m_velocity = next - m_position;

    //     move(env);
    //     m_pathIndex++;
    // }
}

void Agent::chooseAlternativeDirection(Environment &env)
{
    // std::vector<Vector2D> directions = {
    //     {1,0}, {-1,0}, {0,1}, {0,-1} // For moving one step left, right bottom or top
    // };

    // for(const auto& dir : directions)
    // {
    //     Vector2D nextPos = m_position + dir;

    //     if(env.isInsideBounds(nextPos) && !env.isObstacle(nextPos))
    //     {
    //         m_velocity = dir;
    //         return;
    //     }
    // }

    // Pehle dekho ke agent jana kahan chahta tha
    Vector2D desiredDir = computeDirectionToTarget();
    std::vector<Vector2D> priorityDirections;

    // Agar horizontally jana chahta tha aur wall aagayi,
    // to pehle slide (Up/Down) try karo, peechay (reverse) jana aakhri option!
    if (desiredDir.m_x != 0)
    {
        priorityDirections = {{0, 1}, {0, -1}, {-desiredDir.m_x, 0}};
    }
    // Agar vertically hit kiya hai, to Left/Right slide try karo
    else
    {
        priorityDirections = {{1, 0}, {-1, 0}, {0, -desiredDir.m_y}};
    }

    for (const auto &dir : priorityDirections)
    {
        Vector2D nextPos = m_position + dir;

        if (env.isInsideBounds(nextPos) && !env.isObstacle(nextPos))
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
    if (std::abs(delta.m_x) > std::abs(delta.m_y))
        return Vector2D(((delta.m_x > 0) ? 1 : -1), 0); // Move Horizontally
    else if (delta.m_y != 0)
        return Vector2D(0, ((delta.m_y > 0) ? 1 : -1)); // Move Vertically

    return Vector2D(0, 0);
}

void Agent::computePath(Environment &env)
{
    m_path = BFS::findPath(m_position, m_target, env);

    if (m_path.empty())
    {
        std::cout << "No path found! Target is physically unreachable.\n";
        // Do NOT set m_usePathfinding = false;
        // Just leave the path empty.
    }
    else
    {
        // FIX: Start at index 1, because index 0 is the cell the agent is already standing on!
        m_pathIndex = 1;
    }
}